#pragma once
#include "Protocol.h"
#include <map>
#include <vector>
#include <utility>//pair
#include "StateMachine/Bufferable.h"
using namespace std;


class HttpToken
{
public:
    static constexpr const char* CONTENT_LENGTH = "Content-Length";
    static constexpr const char* CONNECTION = "Connection";
    //used by websocket
    static constexpr const char* SEC_WEBSOCKET_KEY = "Sec-WebSocket-Key";
};

class HttpProviderAddress;
class HttpRequest
{
public:
    int client;
    string method;
    string uri;
    string http_version;
    map<string, string> headers;
    string body;

    //转发包
    string forward_packet(const HttpProviderAddress& target_address);
    bool get_path(string& path);
};

class HttpResponse
{
public:

    int status_code;
    string reason_phrase;
    vector<pair<string, string>> headers;
    string body;

    string http_version = "HTTP/1.1";

    HttpResponse() {};

    HttpResponse(const HttpRequest& r);
    virtual string to_string();

    virtual ~HttpResponse() {}
};


class IHttp
{
public:
    //reused by websocket
    virtual HttpRequest parse_request(int client, StreamReader& s) = 0;
    virtual bool parse_chunked_body(StreamReader& s, HttpResponse& response) = 0;

    virtual bool close_if_necessary(HttpRequest& request) = 0;
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

    virtual HttpRequest parse_request(int client, StreamReader& s) override;
    virtual bool parse_chunked_body(StreamReader& s, HttpResponse& response) override;

    virtual bool close_if_necessary(HttpRequest& request) override;
};
