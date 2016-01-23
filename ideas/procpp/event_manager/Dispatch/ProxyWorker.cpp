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


}

void ProxyWorker::on_message(int client, string message, int remote_fd)
{
}

void ProxyWorker::on_close(int client, int remote_fd)
{
}

void ProxyWorker::on_remote_connect(int remote_fd, ConnectResult r, int client)
{
}

void ProxyWorker::on_remote_message(int remote_fd, string message, int client)
{
}

void ProxyWorker::on_remote_close(int remote_fd, int client)
{
}

