#pragma once
#include "ReactHandler.h"
#include <vector>

class IProxyWorker
{
};


class ProxyWorker : public IBaseWorker, public StateBuffer<IBaseWorker::ConnectState>
{
public:
	using AddrList = vector<Utils::SocketAddress>;

	ProxyWorker(ClientServer& server, const AddrList l) : _server(server), _remote_address_list(l) {}

	virtual void on_connect(int client) override;
	virtual void on_message(int client, string message, int remote_fd) override;
	virtual void on_close(int client, int remote_fd) override;

	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) override;
	virtual void on_remote_message(int remote_fd, string message, int client) override;
	virtual void on_remote_close(int remote_fd, int client) override;

	virtual void handle(int fd) override;

private:
	void _erase_pair_info(int client, int remote_fd);

	unsigned int _rr_index = 0;

	ClientServer& _server;
	map<int, int> _c2r;
	map<int, int> _r2c;

	AddrList _remote_address_list;
};
