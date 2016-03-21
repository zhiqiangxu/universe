#pragma once
#include "Protocol.h"
using namespace std;


class IHttp
{
public:
};

class Http : public Protocol, public IHttp, public Stateful<SocksState>
{
public:
	using Protocol::Protocol;

	virtual void on_connect(int fd) override;
	virtual void on_message(int fd, string message) override;
	virtual void on_close(int fd) override;

};
