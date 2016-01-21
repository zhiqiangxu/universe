#pragma once
#include "Protocol.h"
#include <string>
#include <map>
using namespace std;

// this is home made P2P

class IEasyP2P
{
public:
	virtual void on_list(int fd);
	virtual void on_info(int fd, string target);
	virtual void on_connect_target(int fd, string target);
};

class EasyP2P : public Protocol, public IEasyP2P
{
public:
	virtual void on_list(int fd) override;
	virtual void on_info(int fd, string target) override;
	virtual void onc_connect_target(int fd, string target) override;

	virtual void on_connect(int fd) override;
	virtual void on_message(int fd, string message) override;
	virtual void on_close(int fd) override;


private:
	map<int, Utils::SocketAddress> _sock_addrs;
};
