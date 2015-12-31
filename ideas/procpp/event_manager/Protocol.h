#pragma once
#include <string>
#include "Server.h"

using namespace std;


class IProtocol
{
public:
	//TODO why virtual-move-assign
	//virtual void setServer(Server& server) = 0;
	virtual Server& getServer() = 0;
	virtual void handle(int fd) = 0;
};

class Protocol : public IProtocol
{
public:
	static string read(int fd);//fd为ET
	static string read(int fd, size_t size);
	static bool read(string& message, size_t size, string& result);
	static string readUtil(int fd, string separator);
	static bool readUtil(string& message, string separator, string& result, int scanned = 0);

	//TODO why virtual-move-assign
	//virtual void setServer(Server& server) override { _server = server; };
	virtual Server& getServer() override { return _server; };
	/*未实现handle，是抽象类*/

	Protocol(Server& server) : _server(server) {}
protected:
	Server& _server;
};
