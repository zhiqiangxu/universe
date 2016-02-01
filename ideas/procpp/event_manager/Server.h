#pragma once
#include "Scheduler.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <linux/un.h>//struct sockaddr_un
#include "Protocol.h"
#include "UProtocol.h"
#include "Server/Protocol/Global.h"

class IServer
{
public:

	virtual bool listen(uint16_t port, Protocol& proto, int domain) = 0;
	virtual bool listen(uint16_t port, EventManager::EventCB callbacks, int domain) = 0;

	virtual bool listen(const struct sockaddr *addr, socklen_t addrlen, Protocol& proto) = 0;
	virtual bool listen(const struct sockaddr *addr, socklen_t addrlen, EventManager::EventCB callbacks, int type) = 0;

	virtual bool listen(string sun_path, Protocol& proto) = 0;
	virtual bool listen(string sun_path, EventManager::EventCB callbacks) = 0;

	virtual bool listen_u(uint16_t port, UProtocol& proto, int domain) = 0;
	virtual bool listen_u(uint16_t port, EventManager::CB cb, int domain) = 0;
	virtual bool listen_u(uint16_t port, EventManager::EventCB callbacks, int domain) = 0;

	virtual bool listen_u(const struct sockaddr *addr, socklen_t addrlen, UProtocol& proto) = 0;
	virtual bool listen_u(const struct sockaddr *addr, socklen_t addrlen, EventManager::EventCB callbacks) = 0;

	virtual bool listen_u(string sun_path, UProtocol& proto) = 0;
	virtual bool listen_u(string sun_path, EventManager::CB cb) = 0;

	//自动KEEPALIVE,session_id
	virtual int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) = 0;


};

class Server : public IServer, public virtual Scheduler
{
public:

	virtual bool listen(uint16_t port, Protocol& proto, int domain = AF_INET) override;
	virtual bool listen(uint16_t port, EventManager::EventCB callbacks, int domain = AF_INET) override;

	virtual bool listen(const struct sockaddr *addr, socklen_t addrlen, Protocol& proto) override;
	virtual bool listen(const struct sockaddr *addr, socklen_t addrlen, EventManager::EventCB callbacks, int type = SOCK_STREAM) override;

	virtual bool listen(string sun_path, Protocol& proto) override;
	virtual bool listen(string sun_path, EventManager::EventCB callbacks) override;

	virtual bool listen_u(uint16_t port, UProtocol& proto, int domain = AF_INET) override;
	virtual bool listen_u(uint16_t port, EventManager::CB cb, int domain = AF_INET) override;
	virtual bool listen_u(uint16_t port, EventManager::EventCB callbacks, int domain = AF_INET) override;

	virtual bool listen_u(const struct sockaddr *addr, socklen_t addrlen, UProtocol& proto) override;
	virtual bool listen_u(const struct sockaddr *addr, socklen_t addrlen, EventManager::EventCB callbacks) override;

	virtual bool listen_u(string sun_path, UProtocol& proto) override;
	virtual bool listen_u(string sun_path, EventManager::CB cb) override;

	virtual int accept(int socketfd, struct sockaddr *addr, socklen_t *addrlen) override;




private:
	virtual EventManager::EventCB _to_callbacks(Protocol& proto);
	virtual EventManager::EventCB _to_callbacks(UProtocol& proto);
	virtual EventManager::EventCB _to_callbacks_u(EventManager::CB cb);


};

