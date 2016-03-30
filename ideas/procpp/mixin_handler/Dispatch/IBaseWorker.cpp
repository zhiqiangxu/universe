#include "ReactHandler.h"

template<>
const char* Utils::enum_strings<ConnectState>::data[] = {"B4CONNECT", "CONNECT", "CONNECTED"};

void IBaseWorker::handle(int fd)
{
	while (true) {
		auto client = _server.accept(fd, nullptr, nullptr);
		if (client == -1) {
			return;
		}

		on_connect(client);
	}

}

void IBaseWorker::on_message(int client, string message, int remote_fd)
{
	L.debug_log("on_message client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));
	//**this** is MUST:  http://stackoverflow.com/a/19129605
	append_buf(client, message);

	DEBUG(this->has_state(client), "on_message fired when client already removed");
	DEBUG(_c2r[client] == remote_fd, "remote_fd of client[" + to_string(client) + "] should be [" + to_string(remote_fd) + "], but got [" + to_string(_c2r[client]) + "]");
	DEBUG(_r2c[remote_fd] == client, "client of remote_fd[" + to_string(remote_fd) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote_fd]) + "]");

	L.debug_log("get_state = " + Utils::enum_string(this->get_state(client)));
	if (need_buf(client, message, this->get_state(client) != ConnectState::CONNECTED)) {
		//L.debug_log("connecting...buffed");
		return;
	}

	//L.debug_log("write to remote_fd " + to_string(remote_fd));
	_server.write(remote_fd, message.data(), message.length());
}

void IBaseWorker::on_close(int client, int remote_fd)
{
	if (_c2r.find(client) == _c2r.end()) return;

	//L.debug_log("on_close client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));

	_erase_pair_info(client, remote_fd);

	_server.close(remote_fd);
}

void IBaseWorker::on_remote_message(int remote_fd, string message, int client)
{

	L.debug_log("on_remote_message: client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));
	//TODO these can go into IBaseWorker..

	DEBUG(this->has_state(client), "on_remote_message fired when client already removed");
	DEBUG(_c2r[client] == remote_fd, "remote_fd of client[" + to_string(client) + "] should be [" + to_string(remote_fd) + "], but got [" + to_string(_c2r[client]) + "]");
	DEBUG(_r2c[remote_fd] == client, "client of remote_fd[" + to_string(remote_fd) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote_fd]) + "]");

	_server.write(client, message.data(), message.length());

}

void IBaseWorker::on_remote_close(int remote_fd, int client)
{
	L.debug_log("on_remote_close: enter");

	if (_c2r.find(client) == _c2r.end()) return;

	//L.debug_log("on_remote_close client = " + to_string(client) + " remote_fd = " + to_string(remote_fd));

	_erase_pair_info(client, remote_fd);

	_server.close(client);

}

void IBaseWorker::_erase_pair_info(int client, int remote_fd)
{
	erase_state_buffer(client);

	DEBUG(_c2r[client] == remote_fd, "remote_fd of client[" + to_string(client) + "] should be [" + to_string(remote_fd) + "], but got [" + to_string(_c2r[client]) + "]");
	DEBUG(_r2c[remote_fd] == client, "client of remote_fd[" + to_string(remote_fd) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote_fd]) + "]");

	_c2r.erase(client);
	_r2c.erase(remote_fd);
}
