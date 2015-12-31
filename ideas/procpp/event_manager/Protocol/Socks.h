#pragma once
#include "Protocol.h"
#include <map>
using namespace std;

enum class SocksState
{
	GREETING,
	REQUEST,
	CONNECT
};

class ISocks
{
public:
	virtual void on_message(int fd, string message) = 0;
	virtual void on_close(int fd) = 0;
	virtual void close(int fd) = 0;
	virtual bool need_buf(int fd, string& message, bool cond) = 0;
};

//socks5,不支持认证,只支持connect
class Socks : public Protocol, public ISocks
{
public:
	using Protocol::Protocol;

	virtual handle(int fd) override;
	virtual void on_message(int fd, string message) override;
	virtual void on_close(int fd) override;
	virtual void close(int fd) override;
	virtual bool need_buf(int fd, string& message, bool cond) override;

private:
	map<int, string> _buf;
	map<int, SocksState> _state;
};
