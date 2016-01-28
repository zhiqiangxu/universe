#include "Client.h"
#include <strings.h>//bzero
#include <netdb.h>//getaddrinfo
#include <unistd.h>//close
#include <linux/un.h>//struct sockaddr_un
#include <iostream>
#include "ReactHandler.h"
using namespace std;








int Client::connect(string address, uint16_t port, EventManager::EventCB callbacks)
{
	//domain name
	struct addrinfo hints;
	struct addrinfo *result;
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	auto port_string = to_string(port);
	if (getaddrinfo(address.c_str(), port_string.c_str(), &hints, &result) != 0) {
		L.error_log("getaddrinfo");
		return -1;
	}

	int sock;
	for (auto rp = result; rp != nullptr; rp = rp->ai_next) {
		if ( (sock = connect(rp->ai_addr, rp->ai_addrlen, callbacks)) > 0 ) {
			freeaddrinfo(result);
			return sock;
		}
	}

	return -1;
}

int Client::connect(const string sun_path, EventManager::EventCB callbacks)
{
	auto addr = Utils::addr_sun(sun_path);
	return connect(reinterpret_cast<const struct sockaddr*>(&addr), sizeof(addr), callbacks);
}

int Client::connect(const struct sockaddr_un* addr, EventManager::EventCB callbacks, bool async)
{
	return connect(reinterpret_cast<const struct sockaddr*>(addr), sizeof(*addr), callbacks, async);
}

int Client::connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async)
{
	return connect(reinterpret_cast<const struct sockaddr*>(addr), addrlen, EventManager::EventCB{
		{
			EventType::CONNECT, EventManager::CB([this, callbacks, sa_family=addr->sa_family] (int remote_fd, ConnectResult r) mutable {
				auto f = callbacks[EventType::CONNECT];
				callbacks.erase(EventType::CONNECT);
				f(remote_fd, r);

				if (r == ConnectResult::OK) {

					//AF_UNIX不需要keepalive
					if ( sa_family != AF_UNIX ) Utils::set_keepalive(remote_fd);

					if (callbacks.size()) watch(remote_fd, move(callbacks));

				} else {

					close(remote_fd, true);

				}
			})
		}
	}, true, -1);

}

int Client::connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks)
{
	auto s = socket(addr->sa_family, SOCK_STREAM, 0);
	if (s == -1) L.error_exit("socket");

	if (::connect(s, addr, addrlen) == -1) {
		::close(s);
		L.error_log("connect");
		return -1;
	}

	if (callbacks.find(EventType::CONNECT) != callbacks.end()) {
		callbacks[EventType::CONNECT](s);
		callbacks.erase(EventType::CONNECT);
	}

	if (addr->sa_family != AF_UNIX) Utils::set_keepalive(s);

	//刚注册的fd，如果有事件，不会遗漏
	if (callbacks.size()) watch(s, move(callbacks));

	return s;
}


int Client::connect(string address, uint16_t port, EventManager::EventCB callbacks, bool async)
{
	//domain name
	struct addrinfo hints;
	struct addrinfo** resultp = new struct addrinfo*;
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = 0;          /* Any protocol */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	//TODO getaddrinfo_a彻底异步化
	auto port_string = to_string(port);
	if (getaddrinfo(address.c_str(), port_string.c_str(), &hints, resultp) != 0) {
		delete resultp;
		L.error_log("getaddrinfo");
	}

	auto result = *resultp;
	if (!result) {
		delete resultp;
		callbacks[EventType::CONNECT](-1, ConnectResult::NG);
		return -1;
	}

	auto s = Utils::nonblock_socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) L.error_exit("Utils::nonblock_socket");

	auto cb = new function<void(int, ConnectResult)>;

	auto rpp = new struct addrinfo*;

	*rpp = result;
	*cb = [resultp, rpp, this, callbacks, cb, s] (int fd, ConnectResult r) mutable {
		auto result = *resultp;
		if (r == ConnectResult::GAME_OVER) {

CONNECT_FAIL:
			callbacks[EventType::CONNECT](fd, r);
			freeaddrinfo(result);
			delete resultp;
			delete rpp;
			delete cb;
			close(s, true);

		} else if (r == ConnectResult::OK) {

			freeaddrinfo(result);
			delete resultp;
			delete rpp;
			delete cb;
			callbacks[EventType::CONNECT](fd, ConnectResult::OK);
			callbacks.erase(EventType::CONNECT);
			
			//TODO udp ?
			Utils::set_keepalive(fd);

			if (callbacks.size()) watch(fd, move(callbacks));

		} else if (r == ConnectResult::NG) {

			auto rp = *rpp;
			rp = rp->ai_next;
			if (rp == nullptr) goto CONNECT_FAIL;
			else {
				*rpp = rp;
				connect(rp->ai_addr, rp->ai_addrlen, EventManager::EventCB{
					{
						EventType::CONNECT, EventManager::CB(*cb)
					}
				}, true, s);
			}
			
		}
	};

	connect(result->ai_addr, result->ai_addrlen, EventManager::EventCB{
		{
			EventType::CONNECT, EventManager::CB(*cb)
		}
	}, true, s);

	cout << Utils::GREEN("place holder fd is " + to_string(s)) << endl;
	return s;
}

int Client::connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async, int fd)
{
	auto s = Utils::nonblock_socket(addr->sa_family, SOCK_STREAM, 0);
	if (s == -1) L.error_exit("Utils::nonblock_socket");

	if (fd != -1) {
		auto ret = dup2(s, fd);
		if (ret == -1) L.error_exit("dup2");
		::close(s);
		s = fd;
	}

	auto ret = ::connect(s, addr, addrlen);

	//失败，关闭socket
	if (ret == -1 && errno != EINPROGRESS) {
		L.error_log("connect");

		unwatch(s, true);
		callbacks[EventType::CONNECT](s, ConnectResult::NG);
		return -1;
	}

	//只有连接localhost时才有可能出现立即成功
	if (ret == 0) {
		callbacks[EventType::CONNECT](s, ConnectResult::OK);
		callbacks.erase(EventType::CONNECT);
		if (callbacks.size()) watch(s, move(callbacks), true);
	}
	else {
		watch(s, move(callbacks), true);
	}

	return s;
}

