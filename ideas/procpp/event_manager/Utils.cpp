#include "Utils.h"
#include "ReactHandler.h"


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

