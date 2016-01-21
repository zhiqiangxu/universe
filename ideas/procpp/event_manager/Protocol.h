#pragma once
#include <string>
#include "StateMachine/Bufferable.h"

class Server;

using namespace std;


class IProtocol
{
public:
	virtual Server& getServer() = 0;

	/****事件系****/
	virtual void on_connect(int client) {};
	virtual void on_message(int client, string message) = 0;
	virtual void on_close(int client) {};


};

class Protocol : public IProtocol, public Bufferable
{
public:
	static string read(int fd);//fd为ET
	static string read(int fd, size_t size);
	static bool read(string& message, size_t size, string& result);
	static string read_until(int fd, string separator);
	static bool read_until(string& message, string separator, string& result, int scanned = 0);


	virtual Server& getServer() override { return _server; };
	/*未实现on_message，是抽象类*/

	Protocol(Server& server) : _server(server) {}
protected:
	Server& _server;

};
