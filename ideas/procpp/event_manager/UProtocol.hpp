#pragma once
#include <sys/types.h>
#include <sys/socket.h>


class IUProtocol
{
public:
	virtual Server& getServer() = 0;
	virtual void on_message(int u_sock, string message, Utils::SocketAddress addr, socklen_t addrlen) = 0;

};

class UProtocol : public IUProtocol
{
public:

	virtual Server& getServer() override { return _server; };

	/*未实现on_message，是抽象类*/

	UProtocol(Server& server) : _server(server) {}

protected:
	Server& _server;
};

