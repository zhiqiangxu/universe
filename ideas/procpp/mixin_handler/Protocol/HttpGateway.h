#pragma once

#include "Protocol/Http.h"
#include <map>
#include <utility>//pair,make_pair
using namespace std;

class HttpProviderAddress
{
public:
    string host;
    int port;
    string path;
};

enum class HttpGatewayState
{
    PARSE_REQUEST,
    PARSE_RESPONSE,
};

class IHttpGateway
{

	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) = 0;
	virtual void on_remote_message(int remote_fd, string message, int client) = 0;
	virtual void on_remote_close(int remote_fd, int client) = 0;
    //主动关闭远程链接
    virtual void close_remote(int remote_fd) = 0;

	virtual HttpResponse parse_response(int remote_fd, StreamReader& s) = 0;
};

class HttpGateway : public Http, public IHttpGateway, public StateBuffer<HttpGatewayState>
{
public:
    class ON_REQUEST_URI {};

    HttpGateway(ClientServer& server) : Http(server), _server(server) {}

	virtual void on_connect(int client) override;
	virtual void on_close(int client) override;
	virtual void on_message(int client, string message) override;
	virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) override;
	virtual void on_remote_message(int remote_fd, string message, int client) override;
	virtual void on_remote_close(int remote_fd, int client) override;
    virtual void close_remote(int remote_fd) override;

    virtual HttpResponse parse_response(int remote_fd, StreamReader& s) override;

private:
    map<int, int> _c2r;
    map<int, int> _r2c;
    map<int, pair<HttpRequest, HttpProviderAddress>> _forward_info;//维护转发信息
    ClientServer& _server;
};

