#pragma once
#include "EventManager.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/un.h>//struct sockaddr_un
#include "Protocol.h"
#include "Server/Protocol/Global.h"

class IServer
{
public:

	virtual bool listen(uint16_t port, Protocol& proto, int domain) = 0;
	virtual bool listen(uint16_t port, EventManager::EventCB callbacks, int domain) = 0;

	virtual bool listen(const struct sockaddr *addr, socklen_t addrlen, Protocol& proto) = 0;
	virtual bool listen(const struct sockaddr *addr, socklen_t addrlen, EventManager::EventCB callbacks) = 0;

	virtual bool listen(string sun_path, Protocol& proto) = 0;
	virtual bool listen(string sun_path, EventManager::EventCB callbacks) = 0;

	virtual bool listen_u(uint16_t port, Protocol& proto, int domain) = 0;
	virtual bool listen_u(const struct sockaddr *addr, socklen_t addrlen, Protocol& proto) = 0;
	virtual bool listen_u(string sun_path, Protocol& proto) = 0;

	//自动KEEPALIVE,session_id
	virtual int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) = 0;


};

class Server : public IServer, public virtual EventManager
{
public:

	virtual bool listen(uint16_t port, Protocol& proto, int domain = AF_INET) override;
	virtual bool listen(uint16_t port, EventManager::EventCB callbacks, int domain = AF_INET) override;

	virtual bool listen(const struct sockaddr *addr, socklen_t addrlen, Protocol& proto) override;
	virtual bool listen(const struct sockaddr *addr, socklen_t addrlen, EventManager::EventCB callbacks) override;

	virtual bool listen(string sun_path, Protocol& proto) override;
	virtual bool listen(string sun_path, EventManager::EventCB callbacks) override;

	virtual bool listen_u(uint16_t port, Protocol& proto, int domain = AF_INET) override { return false; };
	virtual bool listen_u(const struct sockaddr *addr, socklen_t addrlen, Protocol& proto) override { return false; };
	virtual bool listen_u(string sun_path, Protocol& proto) override { return false; };

	virtual int accept(int socketfd, struct sockaddr *addr, socklen_t *addrlen) override;




private:
	virtual EventManager::EventCB to_callbacks(Protocol& proto);


};

