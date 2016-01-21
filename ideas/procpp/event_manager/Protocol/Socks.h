#pragma once
#include "Protocol.h"
#include "StateMachine/Stateful.h"
#include <map>
using namespace std;

enum class SocksState
{
	GREETING,
	REQUEST,
	CONNECTING,
	CONNECTED
};

class ISocks
{
public:
	virtual void close(int fd) = 0;
	virtual bool send_peer(int peer_fd, string& message) = 0;
};

//socks5,不支持认证,只支持connect
class Socks : public Protocol, public ISocks, public Stateful<SocksState>
{
public:
	using Protocol::Protocol;

	virtual void on_connect(int fd) override;
	virtual void on_message(int fd, string message) override;
	virtual void on_close(int fd) override;

	virtual void close(int fd) override;
	virtual bool send_peer(int peer_fd, string& message) override;

	virtual void erase_state_buffer(int fd);

private:
	map<int, string> _url;
	map<int, int> _c2r;
	map<int, int> _r2c;
};
