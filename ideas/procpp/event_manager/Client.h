#pragma once
#include "EventManager.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string>
using namespace std;

class IClient
{
public:
	virtual bool connect(string address, uint16_t port, EventManager::EventCB callbacks) = 0;
	virtual bool connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks) = 0;
	virtual void start() = 0;
};

class Client : public IClient
{
public:
	virtual bool connect(string address, uint16_t port, EventManager::EventCB callbacks) override;
	virtual bool connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks) override;
	virtual void start() override;

	EventManager& getEventManager() { return _m; }

private:
	EventManager _m;
};

