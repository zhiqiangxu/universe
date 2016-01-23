#include "Dispatch/ProxyWorker.h"


void ProxyWorker::handle(int fd)
{
	while (true) {
		auto client = _server.accept(fd, nullptr, nullptr);
		if (client == -1) {
			return;
		}

		on_connect(client);
	}

}

void ProxyWorker::on_connect(int client)
{
	this->set_state(client, IBaseWorker::ConnectState::B4CONNECT);

	auto addr_index = _rr_index++ % _remote_address_list.size();

	auto addr = _remote_address_list[addr_index];

	L.debug_log(Utils::get_name_info(addr));
	auto remote_fd = _server.connect(reinterpret_cast<const struct sockaddr*>(&addr), Utils::addr_size(addr), EventManager::EventCB{
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

void ProxyWorker::on_message(int client, string message, int remote_fd)
{
	L.debug_log("on_message: client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));

	append_buf(client, message);

	DEBUG(this->has_state(client), "on_message fired when client already removed");
	DEBUG(_c2r[client] == remote_fd, "remote_fd of client[" + to_string(client) + "] should be [" + to_string(remote_fd) + "], but got [" + to_string(_c2r[client]) + "]");
	DEBUG(_r2c[remote_fd] == client, "client of remote_fd[" + to_string(remote_fd) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote_fd]) + "]");

	if (need_buf(client, message, this->get_state(client) != IBaseWorker::ConnectState::CONNECTED)) {
		//L.debug_log("connecting...buffed");
		return;
	}

	L.debug_log("on_message: write\n" + message + "\n");

	_server.write(remote_fd, message.data(), message.length());
}

void ProxyWorker::on_close(int client, int remote_fd)
{
	L.debug_log("on_close");

	if (_c2r.find(client) == _c2r.end()) return;

	_erase_pair_info(client, remote_fd);

	_server.close(remote_fd);

}

void ProxyWorker::on_remote_connect(int remote_fd, ConnectResult r, int client)
{
	//TODO these can go into IBaseWorker..

	L.debug_log("on_remote_connect: "  + Utils::enum_string(r) + " client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));

	if (r == ConnectResult::OK) {
		//L.debug_log("client " + to_string(client) + " set_stat CONNECTED");
		this->set_state(client, IBaseWorker::ConnectState::CONNECTED);

		_server.send_session_id(remote_fd, client);

		//trigger on_message once in case buffered
		if (has_buf(client)) {
			on_message(client, get_buf(client), remote_fd);
		}

	} else {
		if (this->get_state(client) == IBaseWorker::ConnectState::B4CONNECT)
			erase_state_buffer(client);//此时pair信息还没生成
		else
			_erase_pair_info(client, remote_fd);

		_server.close(client);
	}

}

void ProxyWorker::on_remote_message(int remote_fd, string message, int client)
{
	L.debug_log("on_remote_message: client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));
	//TODO these can go into IBaseWorker..

	DEBUG(this->has_state(client), "on_remote_message fired when client already removed");
	DEBUG(_c2r[client] == remote_fd, "remote_fd of client[" + to_string(client) + "] should be [" + to_string(remote_fd) + "], but got [" + to_string(_c2r[client]) + "]");
	DEBUG(_r2c[remote_fd] == client, "client of remote_fd[" + to_string(remote_fd) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote_fd]) + "]");

	_server.write(client, message.data(), message.length());

}

void ProxyWorker::on_remote_close(int remote_fd, int client)
{
	L.debug_log("on_remote_close: enter");
	//TODO these can go into IBaseWorker..

	if (_c2r.find(client) == _c2r.end()) return;

	//L.debug_log("on_remote_close client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));

	_erase_pair_info(client, remote_fd);

	_server.close(client);

}

void ProxyWorker::_erase_pair_info(int client, int remote_fd)
{
	erase_state_buffer(client);

	DEBUG(_c2r[client] == remote_fd, "remote_fd of client[" + to_string(client) + "] should be [" + to_string(remote_fd) + "], but got [" + to_string(_c2r[client]) + "]");
	DEBUG(_r2c[remote_fd] == client, "client of remote_fd[" + to_string(remote_fd) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote_fd]) + "]");

	_c2r.erase(client);
	_r2c.erase(remote_fd);
}

