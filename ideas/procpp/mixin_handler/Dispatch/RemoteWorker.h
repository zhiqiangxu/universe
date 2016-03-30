#pragma once
#include "ProxyWorker.h"

class IRemoteWorker
{
};

//only differs with ProxyWorker in session_id
class RemoteWorker : public ProxyWorker
{
public:
	RemoteWorker(ClientServer& server, const AddrList l, uint16_t port);

	virtual void on_connect(int client) override;

	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) override;

};
