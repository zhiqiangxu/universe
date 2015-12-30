#pragma once
#include "EventManager.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

class IServer
{
public:
	virtual bool listen(uint16_t port, EventManager::EventCB callbacks, int domain, const struct sockaddr *addr, socklen_t addrlen) = 0;
	virtual bool listen_u(uint16_t port, EventManager::EventCB callbacks, int domain, const struct sockaddr *addr, socklen_t addrlen) = 0;
	virtual void start() = 0;
};

class Server : public IServer
{
public:
	virtual bool listen(uint16_t port, EventManager::EventCB callbacks, int domain = AF_INET, const struct sockaddr *addr = nullptr, socklen_t addrlen = 0) override;
	virtual bool listen_u(uint16_t port, EventManager::EventCB callbacks, int domain = AF_INET, const struct sockaddr *addr = nullptr, socklen_t addrlen = 0) override { return false; };
	virtual void start() override;
	EventManager& getEventManager() { return _m; }

private:
	bool listen4(uint16_t port, EventManager::EventCB callbacks, const struct sockaddr *addr = nullptr, socklen_t addrlen = 0);
	bool listen6(uint16_t port, EventManager::EventCB callbacks, const struct sockaddr *addr = nullptr, socklen_t addrlen = 0);
	EventManager _m;

	friend class Echo;
	friend class Socks;
};

