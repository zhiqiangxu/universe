#pragma once
#include "ReactHandler.h"
#include <linux/un.h>//struct sockaddr_un
#include <map>

class IProcessWorker
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
};


//why template ?
//because the Proto needs to be based on Server created in listen_then_fork
template <typename Proto>
class ProcessWorker : public IProcessWorker, public Bufferable, public Stateful<IProcessWorker::ConnectState>
{
	static const int NUMBER_CORES = -1;

public:

	ProcessWorker(ClientServer& server, int n = NUMBER_CORES, string sun_path = "/tmp/react_handler.sock");

	virtual void on_connect(int client) override;
	virtual void on_message(int client, string message, int remote_fd) override;
	virtual void on_close(int client, int remote_fd) override;

	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) override;
	virtual void on_remote_message(int remote_fd, string message, int client) override;
	virtual void on_remote_close(int remote_fd, int client) override;

	virtual void handle(int fd);

	virtual void erase_state_buffer(int fd);

private:
	//remove all info about the pair
	void _erase_pair_info(int client, int remote_fd);
	void _set_path(string sun_path);
	//master side
	void _listen_then_fork(int n);

	ClientServer& _server;
	struct sockaddr_un _sockaddr;

	map<int, int> _c2r;
	map<int, int> _r2c;
};

#include "Dispatch/ProcessWorker.hpp"
