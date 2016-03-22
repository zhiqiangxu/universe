#pragma once
#include "Protocol.h"
#include "StateMachine/Bufferable.h"
using namespace std;


class IHttp
{
public:
};

class Http : public Protocol, public IHttp
{
public:
	using Protocol::Protocol;

	virtual void on_connect(int client) override;
	virtual void on_message(int client, string message) override;
	virtual void on_close(int client) override;

};
