#include "Client.h"
#include <strings.h>//bzero
#include <netdb.h>//getaddrinfo
#include <unistd.h>//close
#include <iostream>
using namespace std;

static void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

//https://en.wikipedia.org/wiki/ANSI_escape_code
static string RED(string s)
{
	return "\033[1;31m" + s + "\033[0m";
}
static string GREEN(string s)
{
	return "\033[1;32m" + s + "\033[0m";
}
static string YELLOW(string s)
{
	return "\033[1;33m" + s + "\033[0m";
}

static void error_log(const char *s)
{
	perror(s);
}


bool Client::connect(string address, uint16_t port, EventManager::EventCB callbacks)
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
		error_log("getaddrinfo");
		return false;
	}

	for (auto rp = result; rp != nullptr; rp = rp->ai_next) {
		if (connect(rp->ai_addr, rp->ai_addrlen, callbacks)) {
			freeaddrinfo(result);
			return true;
		}
	}

	return false;
}

bool Client::connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks)
{
	auto s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) error_exit("socket");

	if (::connect(s, addr, addrlen) == -1) {
		error_log("connect");
		return false;
	}

	if (callbacks.find(EventType::CONNECT) != callbacks.end()) {
		callbacks[EventType::CONNECT](s);
		callbacks.erase(EventType::CONNECT);
	}

	watch(s, callbacks);

	return true;
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
		error_log("getaddrinfo");
	}

	auto result = *resultp;
	if (!result) {
		delete resultp;
		callbacks[EventType::CONNECT](-1, ConnectResult::NG);
		return -1;
	}

	auto s = nonblock_socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) error_exit("nonblock_socket");

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
			if (callbacks.size()) watch(fd, callbacks);

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

	cout << GREEN("place holder fd is " + to_string(s)) << endl;
	return s;
}

int Client::connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks, bool async, int fd)
{
	auto s = nonblock_socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) error_exit("nonblock_socket");

	if (fd != -1) {
		auto ret = dup2(s, fd);
		if (ret == -1) error_exit("dup2");
		::close(s);
		s = fd;
	}

	auto ret = ::connect(s, addr, addrlen);

	//失败，关闭socket
	if (ret == -1 && errno != EINPROGRESS) {
		callbacks[EventType::CONNECT](s, ConnectResult::NG);
		return -1;
	}

	//只有连接localhost时才有可能出现立即成功
	if (ret == 0) {
		callbacks[EventType::CONNECT](s, ConnectResult::OK);
		callbacks.erase(EventType::CONNECT);
		if (callbacks.size()) watch(s, callbacks, true);
	}
	else {
		watch(s, callbacks, true);
	}

	return s;
}

