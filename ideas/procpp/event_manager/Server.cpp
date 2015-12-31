#include "Server.h"
#include <stdio.h>//perror
#include <arpa/inet.h>//htons
#include <strings.h>//bzero
#include <iostream>//cout,endl

using namespace std;

static void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

static struct sockaddr_in addr4(uint16_t port)
{
	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	return serveraddr;
}

static struct sockaddr_in6 addr6(uint16_t port)
{
	struct sockaddr_in6 serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin6_family = AF_INET6;
	serveraddr.sin6_port = htons(port);
	serveraddr.sin6_addr = in6addr_any;

	return serveraddr;
}

bool Server::listen(uint16_t port, EventManager::EventCB callbacks, int domain)
{
	switch (domain) {
		case AF_INET:
		{
			auto serveraddr = addr4(port);
			return listen4(&serveraddr, callbacks);
			break;
		}
		case AF_INET6:
		{
			auto serveraddr = addr6(port);
			return listen6(&serveraddr, callbacks);
			break;
		}
	}

	return false;
}

bool Server::listen4(const struct sockaddr_in *addr, EventManager::EventCB callbacks)
{
	auto s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) error_exit("socket");

	const int enable = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) error_exit("setsockopt");

	// bind,listen接受的都是sockaddr类型参数
	if (bind(s, reinterpret_cast<const struct sockaddr*>(addr), sizeof(*addr)) < 0) error_exit("bind");

	if (::listen(s, 100) < 0) error_exit("listen");

	watch(s, callbacks);

	auto port = ntohs(addr->sin_port);
	cout << "listening port " << port << endl;

	return true;
}

bool Server::listen6(const struct sockaddr_in6 *addr, EventManager::EventCB callbacks)
{
	auto s = socket(AF_INET6, SOCK_STREAM, 0);
	if (s == -1) error_exit("socket");

	const int enable = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) error_exit("setsockopt");

	// bind,listen接受的都是sockaddr类型参数
	if (bind(s, reinterpret_cast<const struct sockaddr*>(addr), sizeof(*addr)) < 0) error_exit("bind");

	if (::listen(s, 100) < 0) error_exit("listen");

	watch(s, callbacks);

	auto port = ntohs(addr->sin6_port);
	cout << "listening port " << port << endl;

	return true;

}


