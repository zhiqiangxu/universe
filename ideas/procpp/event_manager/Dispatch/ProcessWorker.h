#pragma once
#include "ReactHandler.h"
#include <linux/un.h>//struct sockaddr_un
#include <map>




//why template ?
//because the Proto needs to be based on Server created in listen_then_fork
template <typename Proto>
class ProcessWorker : public IBaseWorker
{
	static const int NUMBER_CORES = -1;

public:
	#define DEFAULT_CHILD_SUN_PATH "/tmp/react_handler_worker.sock"
	#define DEFAULT_PARENT_SUN_PATH "/tmp/react_handler_parent.sock"


	ProcessWorker(ClientServer& server, int n = NUMBER_CORES, string child_sun_path = DEFAULT_CHILD_SUN_PATH, string parent_sun_path = DEFAULT_PARENT_SUN_PATH);
	~ProcessWorker();

	virtual void on_connect(int client) override;

	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) override;



private:
	void _set_path(string child_sun_path, string parent_sun_path);
	//master side
	void _listen_then_fork(int n);

	struct sockaddr_un _child_sockaddr;
	struct sockaddr_un _parent_sockaddr;

};

#include "Dispatch/ProcessWorker.hpp"
