#pragma once
#include "EventManager.h"
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string>
using namespace std;

class IClient
{
public:
	/*********IP PORT*********/
	virtual bool connect(string address, uint16_t port, EventManager::EventCB callbacks) = 0;
	// 如异常，返回-1
	virtual int connect(string address, uint16_t port, EventManager::EventCB callbacks, bool async) = 0;

	/********sun path************/
	virtual int connect(const string sun_path, EventManager::EventCB callbacks) = 0;
	/* TODO
	virtual int connect(string sun_path, EventManager::EventCB callbacks, bool async) = 0;
	virtual int connect(const struct sockaddr_un* addr, EventManager::EventCB callbacks) = 0;
	*/
	virtual int connect(const struct sockaddr_un* addr, EventManager::EventCB callbacks, bool async) = 0;

	/********通用地址*********/
	virtual bool connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks) = 0;

	/******通用异步接口******/
	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async) = 0;

	// 一般不调该接口，实在需要，该接口CONNECT回调必须负责关闭fd和set_keepalive(若需要)
	//TODO keepalive能否在connect前就设定好？
	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async, int fd) = 0;
};

class Client : public IClient, public virtual EventManager
{
public:
	/* 没有async参数的都是同步 */

	virtual bool connect(string address, uint16_t port, EventManager::EventCB callbacks) override;
	virtual int connect(string address, uint16_t port, EventManager::EventCB callbacks, bool async) override;

	virtual int connect(const string sun_path, EventManager::EventCB callbacks) override;
	virtual int connect(const struct sockaddr_un* addr, EventManager::EventCB callbacks, bool async) override;

	virtual bool connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks) override;

	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async) override;

	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async, int fd) override;


private:
};

