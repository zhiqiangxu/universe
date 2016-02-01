#pragma once
#include "IBaseWorker.h"
#include "Utils.h"
#include <vector>

class IProxyWorker
{
};

class ClientServer;

class ProxyWorker : public IBaseWorker
{
public:
	using AddrList = vector<Utils::SocketAddress>;

	ProxyWorker(ClientServer& server, const AddrList l) : IBaseWorker(server), _remote_address_list(l) {}

	virtual void on_connect(int client) override;

	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) override;


protected:

	unsigned int _rr_index = 0;


	AddrList _remote_address_list;
};
