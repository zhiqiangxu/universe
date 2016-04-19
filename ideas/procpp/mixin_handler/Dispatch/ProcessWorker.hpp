#include <unistd.h>//fork
#include "ReactHandler.h"
#include <stdlib.h>//exit
#include <unistd.h>//unlink
#include <sys/types.h>
#include <sys/wait.h>//wait



template <typename Proto>
ProcessWorker<Proto>::ProcessWorker(ClientServer& server, int n, string child_sun_path, string parent_sun_path)
: IBaseWorker(server)
{
	_set_path(child_sun_path, parent_sun_path);
	_listen_then_fork(n);

}


template <typename Proto>
void ProcessWorker<Proto>::on_connect(int client)
{
	//L.debug_log("on_connect client = " + to_string(client));

	this->set_state(client, ConnectState::B4CONNECT);


	auto remote_fd = _server.connect(&_child_sockaddr, EventManager::EventCB{
		{
			EventType::CONNECT, EventManager::CB([this, client] (int remote_fd, ConnectResult r) {
				on_remote_connect(remote_fd, r, client);
			}),
		},
		{
			EventType::READ, EventManager::CB([this, client] (int remote_fd, string message) {
				on_remote_message(remote_fd, message, client);
			})
		},
		{
			EventType::CLOSE, EventManager::CB([this, client] (int remote_fd) {
				on_remote_close(remote_fd, client);
			})
		},

	}, true);

	if (remote_fd == -1) return;//on_remote_connect会关闭client

	// AF_UNIX的情况，一般情况下，此时已经CONNECTED
	if (this->get_state(client) == ConnectState::B4CONNECT) this->set_state(client, ConnectState::CONNECT);

	_server.watch(client, EventManager::EventCB{
		{
			EventType::READ, _server.initial_message_wrapper([this, remote_fd] (int client, string message) {
				on_message(client, message, remote_fd);
			}),
		},
		{
			EventType::CLOSE, EventManager::CB([this, remote_fd] (int client) {
				on_close(client, remote_fd);
			})
		}
	});

	_c2r[client] = remote_fd;
	_r2c[remote_fd] = client;
}


template <typename Proto>
void ProcessWorker<Proto>::on_remote_connect(int remote_fd, ConnectResult r, int client)
{
	//L.debug_log( "on_remote_connect client = " + to_string(client) + " remote_fd = " + to_string(remote_fd) + " result = " + Utils::enum_string(r) );

	if (r == ConnectResult::OK) {

		_server.add_session_task(client, [remote_fd, this] (int client) {

			_server.send_session_id(remote_fd, client);

			//L.debug_log("client " + to_string(client) + " set_stat CONNECTED");
			this->set_state(client, ConnectState::CONNECTED);


			//trigger on_message once in case buffered
			if (has_buf(client)) on_message(client, get_buf(client), remote_fd);

		});

	} else {
		if (this->get_state(client) == ConnectState::B4CONNECT)
			erase_state_buffer(client);//此时pair信息还没生成
		else
			_erase_pair_info(client, remote_fd);

		_server.close(client);
	}
}


template <typename Proto>
void ProcessWorker<Proto>::_set_path(string child_sun_path, string parent_sun_path)
{
	if (Utils::file_exists(child_sun_path)) L.error_exit(child_sun_path + " already exists!");
	if (Utils::file_exists(parent_sun_path)) L.error_exit(parent_sun_path + " already exists!");

	_child_sockaddr = Utils::addr_sun(child_sun_path);
	_parent_sockaddr = Utils::addr_sun(parent_sun_path);
}

template <typename Proto>
void ProcessWorker<Proto>::_listen_then_fork(int n)
{
	auto pm_pid = fork();
	if (pm_pid == -1) L.error_exit("fork");
	if (pm_pid) {
		auto ok = _server.listen_for_child(_parent_sockaddr.sun_path);

		if (!ok) L.error_exit("listen child sock failed");

		return;
	}

	//process manager


	if (n == ProcessWorker::NUMBER_CORES) n = Utils::get_cpu_cores();

	Proto proto(_server);
	auto ok = _server.listen( reinterpret_cast<const struct sockaddr*>(&_child_sockaddr), sizeof(_child_sockaddr), proto );


	if (!ok) L.error_exit("listen child sock failed");

	for (int i = 0; i < n; i++) {
		auto pid = fork();
		if (pid == -1) L.error_exit("fork");

		if (pid) {
		} else {

			_server.fire<EventManager::ON_FORK>();
			_server.set_parent(_parent_sockaddr.sun_path);/* not master, so it should receive session id */

			_server.start();

			// child should never return
			exit(0);
		}
	}


	//restart child
	int status;
	pid_t child;
	while ((child = waitpid(-1, &status, 0)) > 0)
	{
		auto pid = fork();
		if (pid == -1) L.error_exit("fork");

		if (pid) {
		} else {
			_server.fire<EventManager::ON_FORK>();
			_server.set_parent(_parent_sockaddr.sun_path);/* not master, so it should receive session id */

			_server.start();

			// child should never return
			exit(0);
		}
	}

}

template <typename Proto>
ProcessWorker<Proto>::~ProcessWorker()
{
	::unlink(_child_sockaddr.sun_path);
	::unlink(_parent_sockaddr.sun_path);
}
