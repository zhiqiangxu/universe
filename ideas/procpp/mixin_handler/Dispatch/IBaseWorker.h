#pragma once
#include "StateMachine/StateBuffer.h"
#include "EventManager.h"
#include <string>
using namespace std;

// TODO should rename IBaseWorker since it's not the base of all workers now

class ClientServer;

enum class ConnectState
{
	B4CONNECT,
	CONNECT,
	CONNECTED,
};


class IBaseWorker : public StateBuffer<ConnectState>
{
public:
	IBaseWorker(ClientServer& server) : _server(server) {}

	ClientServer& get_server() { return _server; }

	virtual void on_connect(int client) = 0;
	virtual void on_message(int client, string message, int remote_fd);
	virtual void on_close(int client, int remote_fd);

	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) = 0;
	virtual void on_remote_message(int remote_fd, string message, int client);
	virtual void on_remote_close(int remote_fd, int client);

	virtual void handle(int fd);

protected:

	//remove all info about the pair
	virtual void _erase_pair_info(int client, int remote_fd);

	ClientServer& _server;
	map<int, int> _c2r;
	map<int, int> _r2c;

};

