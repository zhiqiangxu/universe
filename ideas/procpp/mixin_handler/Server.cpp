#include "Server.h"
#include <stdio.h>//perror
#include <arpa/inet.h>//htons
#include <strings.h>//bzero
#include <string.h>//memcpy
#include <iostream>//cout,endl
#include "ReactHandler.h"

using namespace std;



bool Server::listen(uint16_t port, Protocol& proto, int domain)
{
	return listen(port, _to_callbacks(proto), domain);
}

bool Server::listen(uint16_t port, EventManager::EventCB callbacks, int domain)
{
	switch (domain) {
		case AF_INET:
		{
			auto serveraddr = Utils::addr4(port);
			return listen(reinterpret_cast<const struct sockaddr*>(&serveraddr), sizeof(serveraddr), callbacks);
		}
		case AF_INET6:
		{
			auto serveraddr = Utils::addr6(port);
			return listen(reinterpret_cast<const struct sockaddr*>(&serveraddr), sizeof(serveraddr), callbacks);
			break;
		}
	}

	return false;
}

bool Server::listen(const struct sockaddr *addr, socklen_t addrlen, Protocol& proto)
{
	return listen(addr, addrlen, _to_callbacks(proto));
}

bool Server::listen(const struct sockaddr *addr, socklen_t addrlen, EventManager::EventCB callbacks, int type)
{
	auto s = socket(addr->sa_family, type, 0);
	if (s == -1) L.error_exit("socket");

	const int enable = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR/* TODO figure its usage*/, &enable, sizeof(enable)) == -1) L.error_exit("setsockopt");

	// bind,listen接受的都是sockaddr类型参数
	if (bind(s, addr, addrlen) < 0) L.error_exit("bind");

	if ( type == SOCK_STREAM && ::listen(s, 100) < 0 ) L.error_exit("listen");


	if ( !watch(s, move(callbacks)) ) L.error_exit("watch " + to_string(s));

	L.info_log("listen ok");

	return true;
}

bool Server::listen(string sun_path, Protocol& proto)
{
	return listen(sun_path, _to_callbacks(proto));
}

bool Server::listen(string sun_path, EventManager::EventCB callbacks)
{
	auto serveraddr = Utils::addr_sun(sun_path);

	cout << "listening " << sun_path << endl;

	return listen(reinterpret_cast<struct sockaddr*>(&serveraddr), sizeof(serveraddr), callbacks);
}

int Server::accept(int socketfd, struct sockaddr *addr, socklen_t *addrlen)
{
	static struct sockaddr _addr;
	static socklen_t _addrlen = sizeof(_addr);

	if (addr == nullptr) {
		addr = &_addr;
		addrlen = &_addrlen;
	}

	auto client = ::accept(socketfd, addr, addrlen);
	if (client == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) return -1;

		L.error_exit("accept");
	}

	if (addr->sa_family != AF_UNIX) Utils::set_keepalive(client);

	return client;
}



EventManager::EventCB Server::_to_callbacks(Protocol& proto)
{
	return EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([&proto, this] (int fd) mutable {

				while (true) {
					auto client = accept(fd, nullptr, nullptr);
					if (client == -1) {
						cout << "[accept] end" << endl;
						return;
					}

					proto.on_connect(client);

					watch(client, EventManager::EventCB{
						{
							EventType::READ, EventManager::CB([&proto] (int client, string message) mutable {
								proto.on_message(client, message);

							}),
						},
						{
							EventType::CLOSE, EventManager::CB([&proto] (int client) {
								proto.on_close(client);
							})
						}
					});

				}

			})
		}
	};
}

/**********************TCP END**********************************/
/**********************UDP START********************************/
bool Server::listen_u(const struct sockaddr *addr, socklen_t addrlen, EventManager::EventCB callbacks)
{
	return listen(addr, addrlen, callbacks, SOCK_DGRAM);
}

bool Server::listen_u(const struct sockaddr *addr, socklen_t addrlen, UProtocol& proto)
{
	return listen_u(addr, addrlen, _to_callbacks(proto));
}

bool Server::listen_u(uint16_t port, UProtocol& proto, int domain)
{
	return listen_u(port, _to_callbacks(proto), domain);
}

bool Server::listen_u(uint16_t port, EventManager::CB cb, int domain)
{
	return listen_u(port, _to_callbacks_u(cb), domain);
}

bool Server::listen_u(uint16_t port, EventManager::EventCB callbacks, int domain)
{
	switch (domain) {
		case AF_INET:
		{
			auto serveraddr = Utils::addr4(port);
			return listen_u(reinterpret_cast<const struct sockaddr*>(&serveraddr), sizeof(serveraddr), callbacks);
		}
		case AF_INET6:
		{
			auto serveraddr = Utils::addr6(port);
			return listen_u(reinterpret_cast<const struct sockaddr*>(&serveraddr), sizeof(serveraddr), callbacks);
			break;
		}
	}

	return false;
}

bool Server::listen_u(string sun_path, UProtocol& proto)
{
	auto serveraddr = Utils::addr_sun(sun_path);

	return listen_u(reinterpret_cast<const struct sockaddr*>(&serveraddr), sizeof(serveraddr), proto);
}

bool Server::listen_u(string sun_path, EventManager::CB cb)
{
	auto serveraddr = Utils::addr_sun(sun_path);

	return listen_u(reinterpret_cast<const struct sockaddr*>(&serveraddr), sizeof(serveraddr), _to_callbacks_u(cb));
}

EventManager::EventCB Server::_to_callbacks(UProtocol& proto)
{
	auto buffer_size = proto.get_buffer_size();
	auto buffer = new char[buffer_size];

	return EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([&proto, buffer_size, buffer, this] (int u_sock) mutable {

				Utils::SocketAddress addr;
				socklen_t addrlen = sizeof(addr);
				while(true) {

					auto ret = recvfrom(u_sock, buffer, buffer_size, 0, reinterpret_cast<struct sockaddr *>(&addr), &addrlen);
					if (ret < 0) {
						L.debug_log("recvfrom finish");
						return;
					}

					string message("");
					message.assign(buffer, ret);
					proto.on_message(u_sock, message, addr, addrlen);

				}

			})
		},
		{
			EventType::CLOSE, EventManager::CB([buffer] (int fd) {
				delete [] buffer;
			})
		}
	};
}

EventManager::EventCB Server::_to_callbacks_u(EventManager::CB cb)
{
	return EventManager::EventCB{
		{
			EventType::READ, cb
		}
	};
}