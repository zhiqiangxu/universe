#pragma once
#include "ReactHandler.h"
#include <linux/un.h>//struct sockaddr_un
#include <map>




//why template ?
//because the Proto needs to be based on Server created in listen_then_fork
template <typename Proto>
class ProcessWorker : public IBaseWorker, public StateBuffer<IBaseWorker::ConnectState>
{
	static const int NUMBER_CORES = -1;

public:
	#define DEFAULT_CHILD_SUN_PATH "/tmp/react_handler_worker.sock"
	#define DEFAULT_PARENT_SUN_PATH "/tmp/react_handler_parent.sock"


	ProcessWorker(ClientServer& server, int n = NUMBER_CORES, string child_sun_path = DEFAULT_CHILD_SUN_PATH, string parent_sun_path = DEFAULT_PARENT_SUN_PATH);

	virtual void on_connect(int client) override;
	virtual void on_message(int client, string message, int remote_fd) override;
	virtual void on_close(int client, int remote_fd) override;

	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) override;
	virtual void on_remote_message(int remote_fd, string message, int client) override;
	virtual void on_remote_close(int remote_fd, int client) override;

	virtual void handle(int fd) override;


private:
	//remove all info about the pair
	void _erase_pair_info(int client, int remote_fd);
	void _set_path(string child_sun_path, string parent_sun_path);
	//master side
	void _listen_then_fork(int n);

	ClientServer& _server;
	struct sockaddr_un _child_sockaddr;
	struct sockaddr_un _parent_sockaddr;

	map<int, int> _c2r;
	map<int, int> _r2c;
};

#include "Dispatch/ProcessWorker.hpp"
