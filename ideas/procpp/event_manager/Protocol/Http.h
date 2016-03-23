#pragma once
#include "Protocol.h"
#include <map>
#include "StateMachine/Bufferable.h"
using namespace std;


class IHttp
{
public:
};

class HttpRequest
{
public:
	string method;
	string uri;
	string http_version;
	map<string, string> headers;
	string body;
};

class Http : public Protocol, public IHttp
{
public:
	//event tags
	class ON_REQUEST {};

	using Protocol::Protocol;

	virtual void on_connect(int client) override;
	virtual void on_message(int client, string message) override;
	virtual void on_close(int client) override;

};
