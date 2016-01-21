#include "Protocol/EasyP2P.h"
#include "Utils.h"

void EasyP2P::on_connect(int sock)
{
	Utils::SocketAddress addr;
	if (!Utils::get_peer_name(sock, addr)) {
		_server.close(sock);
		return;
	}

	_sock_addrs[sock] = addr;
}

void EasyP2P::on_message(int sock, string message)
{
}

void EasyP2P::on_close(int sock)
{
	_sock_addrs.erase(sock);
}
