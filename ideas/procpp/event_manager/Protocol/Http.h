#pragma once
#include "Protocol.h"
#include <map>
#include "StateMachine/Bufferable.h"
using namespace std;


class HttpToken
{
public:
	static constexpr const char* CONTENT_LENGTH = "Content-Length";
	static constexpr const char* CONNECTION = "Connection";
};

class IHttp
{
public:
};

class HttpRequest
{
public:
	int client;
	string method;
	string uri;
	string http_version;
	map<string, string> headers;
	string body;
};

class HttpResponse
{
public:

	int status_code = 200;
	string reason_phrase;
	map<string, string> headers;
	string body;

	static constexpr const char* http_version = "HTTP/1.1";

	virtual string to_string();
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