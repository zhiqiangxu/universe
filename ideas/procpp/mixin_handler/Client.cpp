#include <strings.h>//bzero
#include <netdb.h>//getaddrinfo
#include <unistd.h>//close
#include <linux/un.h>//struct sockaddr_un
#include <iostream>
#include <sys/epoll.h>//epoll_wait
#include <sys/select.h>//select
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

	if (callbacks.find(EventType::CONNECT) != callbacks.end()) {
		callbacks[EventType::CONNECT](-1, ConnectResult::NG);
	}

	return -1;
}

int Client::connect(string address, uint16_t port, EventManager::CB::C connect_callback)
{
	return connect(address, port, to_ecb(connect_callback));
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
	return connect(addr, addrlen, EventManager::EventCB{
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

//同步实现口
int Client::connect(const struct sockaddr* addr, socklen_t addrlen, int timeout)
{
	auto s = socket(addr->sa_family, SOCK_STREAM, 0);
	if (s == -1) L.error_exit("socket");

	if (timeout > 0) {
		//SO_SNDTIMEO会影响所有发包操作，所以用异步socket来实现timeout
		Utils::set_nonblock(s);
		auto status = ::connect(s, addr, addrlen);
		if (status == -1 && errno != EINPROGRESS) {
			L.error_log("connect");
			goto FAILURE;
		}

		fd_set wset;
		FD_ZERO(&wset);
		FD_SET(s, &wset);

		auto tv = Utils::to_timeval(timeout);
		status = select(s+1, nullptr, &wset, nullptr, &tv);

		if (status != 1) {
			if (status == -1) L.error_log("select");

			goto FAILURE;
		}

		return s;
	}
	else {
		if (::connect(s, addr, addrlen) == -1) {
			L.error_log("connect");
			goto FAILURE;
		}
	}

	if (addr->sa_family != AF_UNIX) Utils::set_keepalive(s);

	return s;

FAILURE:
	::close(s);
	return -1;


}

int Client::connect(const struct sockaddr* addr, socklen_t addrlen, EventManager::EventCB callbacks)
{

	auto s = connect(addr, addrlen);

	if (callbacks.find(EventType::CONNECT) != callbacks.end()) {
		callbacks[EventType::CONNECT](s, s > 0 ? ConnectResult::OK : ConnectResult::NG);
		callbacks.erase(EventType::CONNECT);
	}

	if (s < 0) return s;

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

int Client::connect(string address, uint16_t port, EventManager::CB::C connect_callback, bool async)
{
	return connect(address, port, to_ecb(connect_callback), async);
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

int Client::wait(const vector<GUID>& requests, int milliseconds)
{
    if (requests.size() == 0) return 0;

    if (count_fds() == 0) return 0;

	auto now = Utils::now();

	const int kMaxEvents = 32;
	struct epoll_event events[kMaxEvents];

	map<GUID, bool> request_map;
	for (auto& uuid : requests) {
		request_map[uuid] = true;
	}

	auto id = on<Client::ON_PACKET_OK>(Utils::to_function([&request_map](GUID& uuid){
		request_map.erase(uuid);
	}));

	while (true) {
		int timeout = milliseconds - chrono::duration_cast<chrono::milliseconds>(Utils::now() - now).count();
		if (timeout < 0) break;

		//cout << "epoll_wait" << endl;
		auto ret = epoll_wait(_epoll_fd, events, kMaxEvents, timeout);
		//cout << "ret " << ret << endl;

		handle_events(ret, events);

		if (request_map.size() == 0) break;
	}

	detach<Client::ON_PACKET_OK, GUID&>(id);

	return requests.size() - request_map.size();
}

int Client::wait(int n, int milliseconds)
{

    if (n <= 0) return 0;

    if (count_fds() == 0) return 0;

    auto total = n;

	auto now = Utils::now();

	const int kMaxEvents = 32;
	struct epoll_event events[kMaxEvents];

	auto id = on<Client::ON_PACKET_OK>(Utils::to_function([&n](GUID& uuid){
        n -= 1;
	}));

	while (true) {
		int timeout = milliseconds - chrono::duration_cast<chrono::milliseconds>(Utils::now() - now).count();
		if (timeout < 0) break;

		//cout << "epoll_wait" << endl;
		auto ret = epoll_wait(_epoll_fd, events, kMaxEvents, timeout);
		//cout << "ret " << ret << endl;

		handle_events(ret, events);

		if (n <= 0) break;
	}

	detach<Client::ON_PACKET_OK, GUID&>(id);

	return total - n;

}

EventManager::EventCB Client::to_callbacks(P::Client::Base& proto)
{
	return EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([&proto, this] (int fd, string message) mutable {
				proto.on_message(fd, message);
			})
		},
		{
			EventType::CLOSE, EventManager::CB([&proto, this] (int fd) mutable {
				proto.on_close(fd);
			})
		},
	};
}
