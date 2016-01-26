#include <unistd.h>//fork
#include "ReactHandler.h"
#include <stdlib.h>//exit



template <typename Proto>
ProcessWorker<Proto>::ProcessWorker(ClientServer& server, int n, string child_sun_path, string parent_sun_path)
: _server(server)
{
	_set_path(child_sun_path, parent_sun_path);
	_listen_then_fork(n);
}

template <typename Proto>
void ProcessWorker<Proto>::handle(int fd)
{
	while (true) {
		auto client = _server.accept(fd, nullptr, nullptr);
		if (client == -1) {
			return;
		}

		on_connect(client);
	}
}


template <typename Proto>
void ProcessWorker<Proto>::on_connect(int client)
{
	//L.debug_log("on_connect client = " + to_string(client));

	this->set_state(client, IBaseWorker::ConnectState::B4CONNECT);


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
	if (this->get_state(client) == IBaseWorker::ConnectState::B4CONNECT) this->set_state(client, IBaseWorker::ConnectState::CONNECT);

	_server.watch(client, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([this, remote_fd] (int client, string message) {
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
void ProcessWorker<Proto>::on_message(int client, string message, int remote_fd)
{
	//L.debug_log("on_message client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));
	//**this** is MUST:  http://stackoverflow.com/a/19129605
	append_buf(client, message);

	DEBUG(this->has_state(client), "on_message fired when client already removed");
	DEBUG(_c2r[client] == remote_fd, "remote_fd of client[" + to_string(client) + "] should be [" + to_string(remote_fd) + "], but got [" + to_string(_c2r[client]) + "]");
	DEBUG(_r2c[remote_fd] == client, "client of remote_fd[" + to_string(remote_fd) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote_fd]) + "]");

	L.debug_log("get_state = " + Utils::enum_string(this->get_state(client)));
	if (need_buf(client, message, this->get_state(client) != IBaseWorker::ConnectState::CONNECTED)) {
		//L.debug_log("connecting...buffed");
		return;
	}

	//L.debug_log("write to remote_fd " + to_string(remote_fd));
	_server.write(remote_fd, message.data(), message.length());
}

template <typename Proto>
void ProcessWorker<Proto>::on_close(int client, int remote_fd)
{
	if (_c2r.find(client) == _c2r.end()) return;

	//L.debug_log("on_close client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));

	_erase_pair_info(client, remote_fd);

	_server.close(remote_fd);
}

template <typename Proto>
void ProcessWorker<Proto>::on_remote_connect(int remote_fd, ConnectResult r, int client)
{
	L.debug_log( "on_remote_connect client = " + to_string(client) + " remote_fd = " + to_string(remote_fd) + " result = " + Utils::enum_string(r) );

	if (r == ConnectResult::OK) {
		//L.debug_log("client " + to_string(client) + " set_stat CONNECTED");
		this->set_state(client, IBaseWorker::ConnectState::CONNECTED);

		_server.send_session_id(remote_fd, client);

		//trigger on_message once in case buffered
		if (this->has_buf(client)) on_message(client, "", remote_fd);

	} else {
		if (this->get_state(client) == IBaseWorker::ConnectState::B4CONNECT)
			erase_state_buffer(client);//此时pair信息还没生成
		else
			_erase_pair_info(client, remote_fd);

		_server.close(client);
	}
}

template <typename Proto>
void ProcessWorker<Proto>::on_remote_message(int remote_fd, string message, int client)
{
	//L.debug_log("on_remote_message client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));

	DEBUG(this->has_state(client), "on_remote_message fired when client already removed");
	DEBUG(_c2r[client] == remote_fd, "remote_fd of client[" + to_string(client) + "] should be [" + to_string(remote_fd) + "], but got [" + to_string(_c2r[client]) + "]");
	DEBUG(_r2c[remote_fd] == client, "client of remote_fd[" + to_string(remote_fd) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote_fd]) + "]");

	_server.write(client, message.data(), message.length());
}

template <typename Proto>
void ProcessWorker<Proto>::on_remote_close(int remote_fd, int client)
{
	if (_c2r.find(client) == _c2r.end()) return;

	//L.debug_log("on_remote_close client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));

	_erase_pair_info(client, remote_fd);

	_server.close(client);
}

template <typename Proto>
void ProcessWorker<Proto>::_erase_pair_info(int client, int remote_fd)
{
	erase_state_buffer(client);

	DEBUG(_c2r[client] == remote_fd, "remote_fd of client[" + to_string(client) + "] should be [" + to_string(remote_fd) + "], but got [" + to_string(_c2r[client]) + "]");
	DEBUG(_r2c[remote_fd] == client, "client of remote_fd[" + to_string(remote_fd) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote_fd]) + "]");

	_c2r.erase(client);
	_r2c.erase(remote_fd);
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
	if (n == ProcessWorker::NUMBER_CORES) n = Utils::get_cpu_cores();

	ClientServer worker_server;
	Proto proto(worker_server);
	auto ok = worker_server.listen( reinterpret_cast<const struct sockaddr*>(&_child_sockaddr), sizeof(_child_sockaddr), proto );


	if (!ok) L.error_exit("listen child sock failed");


	for (int i = 0; i < n; i++) {
		auto pid = fork();
		if (pid == -1) L.error_exit("fork");

		if (pid) {
		} else {
			worker_server.set_parent(_parent_sockaddr.sun_path);/* not master, so it should receive session id */

			worker_server.start();

			// child should never return
			exit(0);
		}
	}

	ok = _server.listen_for_child(_parent_sockaddr.sun_path);

	if (!ok) L.error_exit("listen parent sock failed");
}


