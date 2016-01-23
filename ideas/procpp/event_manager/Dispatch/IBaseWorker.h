#pragma once

#include <string>
using namespace std;


class IBaseWorker
{
public:
	enum class ConnectState
	{
		B4CONNECT,
		CONNECT,
		CONNECTED,
	};


	virtual void on_connect(int client) = 0;
	virtual void on_message(int client, string message, int remote_fd) = 0;
	virtual void on_close(int client, int remote_fd) = 0;

	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) = 0;
	virtual void on_remote_message(int remote_fd, string message, int client) = 0;
	virtual void on_remote_close(int remote_fd, int client) = 0;

	virtual void handle(int fd) = 0;
};

