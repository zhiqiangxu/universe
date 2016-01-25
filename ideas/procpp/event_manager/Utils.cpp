#include "Utils.h"
#include "ReactHandler.h"
//setsockopt
#include <sys/types.h>
#include <sys/socket.h>
//IPPROTO_TCP
#include <netinet/in.h>
//TCP_KEEPIDLE
#include <netinet/tcp.h>
//fcntl
#include <unistd.h>
#include <fcntl.h>



string Utils::get_name_info(const SocketAddress& addr)
{
	static char host[40];
	static char service[20];

	auto addrp = reinterpret_cast<const struct sockaddr*>(&addr);
	if (getnameinfo(addrp, addr_size(addrp->sa_family), host, sizeof(host), service, sizeof(service), 0) != 0) {
		L.error_exit("getnameinfo");
	}

	return string(host) + ":" + string(service);
}

void Utils::set_nonblock(int fd)
{
	auto old_flags = fcntl(fd, F_GETFL);

	if (old_flags & O_NONBLOCK) return;//已经nonblock

	auto new_flags = old_flags | O_NONBLOCK;
	if (fcntl(fd, F_SETFL, new_flags) == -1) L.error_exit("set_nonblock");
}

void Utils::set_keepalive(int socketfd, int keepidle, int keepinterval, int keepcount)
{
	int sockopt = 1;
	if (setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &sockopt, sizeof(int)) == -1) L.error_exit("setsockopt SO_KEEPALIVE");

	if (setsockopt(socketfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(int)) == -1) L.error_exit("setsockopt TCP_KEEPIDLE");

	if (setsockopt(socketfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepinterval, sizeof(int)) == -1) L.error_exit("setsockopt TCP_KEEPINTVL");

	if (setsockopt(socketfd, IPPROTO_TCP, TCP_KEEPCNT, &keepcount, sizeof(int)) == -1) L.error_exit("setsockopt TCP_KEEPCNT");
}

int Utils::nonblock_socket(int domain, int type, int protocol)
{
	auto s = socket(domain, type, protocol);
	if (s == -1) {
		L.error_log("socket");
		return -1;
	}

	set_nonblock(s);
	return s;
}

