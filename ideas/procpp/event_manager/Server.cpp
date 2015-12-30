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


bool Server::listen(uint16_t port, EventManager::EventCB callbacks, int domain, const struct sockaddr *addr, socklen_t addrlen)
{
	switch (domain) {
		case AF_INET:
			return listen4(port, callbacks, addr, addrlen);
			break;
		case AF_INET6:
			return listen6(port, callbacks, addr, addrlen);
			break;
	}

	return false;
}

bool Server::listen4(uint16_t port, EventManager::EventCB callbacks, const struct sockaddr *addr, socklen_t addrlen)
{
	auto s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) error_exit("socket");

	const int enable = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) error_exit("setsockopt");

	if (!addr) {
		struct sockaddr_in serveraddr;
		bzero(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons(port);

		addr = (struct sockaddr *)&serveraddr;
		addrlen = sizeof(serveraddr);
	}

	// bind,listen接受的都是sockaddr类型参数
	if (bind(s, addr, addrlen) < 0) error_exit("bind");

	if (::listen(s, 100) < 0) error_exit("listen");

	_m.watch(s, callbacks);

	cout << "listening port " << port << endl;

	return true;
}

bool Server::listen6(uint16_t port, EventManager::EventCB callbacks, const struct sockaddr *addr, socklen_t addrlen)
{
	auto s = socket(AF_INET6, SOCK_STREAM, 0);
	if (s == -1) error_exit("socket");

	if (!addr) {
		struct sockaddr_in6 serveraddr;
		bzero(&serveraddr, sizeof(serveraddr));
		serveraddr.sin6_family = AF_INET6;
		serveraddr.sin6_port = htons(port);
		serveraddr.sin6_addr = in6addr_any;

		addr = (struct sockaddr *)&serveraddr;
		addrlen = sizeof(serveraddr);
	}

	// bind,listen接受的都是sockaddr类型参数
	if (bind(s, addr, addrlen) < 0) error_exit("bind");

	if (::listen(s, 100) < 0) error_exit("listen");

	_m.watch(s, callbacks);

	cout << "listening port " << port << endl;

	return true;

}

void Server::start()
{
	_m.start();
}
