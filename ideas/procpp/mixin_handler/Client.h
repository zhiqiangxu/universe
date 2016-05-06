#pragma once
#include "Scheduler.h"
#include "Protocol.h"
#include "Utils.h"//GUID
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string>
#include <vector>
using namespace std;

namespace P { namespace Client {
	class Base;
}}

class IClient
{
public:

	/*******************返回值规则：***********************
     ******************************************************
     ************成功返回socket，失败返回-1****************
	 ******************************************************/

	/*********IP PORT*********/
	virtual int connect(string address, uint16_t port, EventManager::EventCB callbacks) = 0;
	virtual int connect(string address, uint16_t port, EventManager::CB::C connect_callback) = 0;
	virtual int connect(string address, uint16_t port, EventManager::EventCB callbacks, bool async) = 0;
	virtual int connect(string address, uint16_t port, EventManager::CB::C connect_callback, bool async) = 0;

	/********sun path************/
	virtual int connect(const string sun_path, EventManager::EventCB callbacks) = 0;
	/* TODO
	virtual int connect(string sun_path, EventManager::EventCB callbacks, bool async) = 0;
	virtual int connect(const struct sockaddr_un* addr, EventManager::EventCB callbacks) = 0;
	*/
	virtual int connect(const struct sockaddr_un* addr, EventManager::EventCB callbacks, bool async) = 0;

	/********通用同步接口*********/
	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, int timeout/*milliseconds*/) = 0;//无回调
	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks) = 0;//有回调

	/******通用异步接口******/
	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async) = 0;

	// 一般不调该接口，实在需要，该接口CONNECT回调必须负责关闭fd和set_keepalive(若需要)
	//TODO keepalive能否在connect前就设定好？
	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async, int fd) = 0;

	//等待网络请求返回
	virtual int wait(int milliseconds, const vector<GUID>& requests) = 0;

};

class Client : public IClient, public virtual Scheduler
{
public:
	/* 没有async参数的都是同步 */

	class ON_PACKET_OK {};

	virtual int connect(string address, uint16_t port, EventManager::EventCB callbacks) override;
	virtual int connect(string address, uint16_t port, EventManager::CB::C connect_callback) override;
	virtual int connect(string address, uint16_t port, EventManager::EventCB callbacks, bool async) override;
	virtual int connect(string address, uint16_t port, EventManager::CB::C connect_callback, bool async) override;

	virtual int connect(const string sun_path, EventManager::EventCB callbacks) override;
	virtual int connect(const struct sockaddr_un* addr, EventManager::EventCB callbacks, bool async) override;

	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, int timeout = -1/*milliseconds*/) override;
	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks) override;

	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async) override;

	virtual int connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async, int fd) override;

	virtual int wait(int milliseconds, const vector<GUID>& requests) override;

	virtual EventManager::EventCB to_callbacks(P::Client::Base& proto);

private:
};

