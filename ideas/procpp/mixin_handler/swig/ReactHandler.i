%module(directors="1") ReactHandler

%{
#include "ReactHandler.h"
%}

%include "stdint.i"
%include "std_string.i"
%include "std_map.i"

using namespace std;
namespace std
{
    %template(StringMap) map<string, string>;
}

/*********************
***** WebSocket ******
*********************/

class WebSocketMessage
{
public:
	int client;
	uint8_t opcode;
	string payload;
};

%nodefaultctor WebSocket;

class WebSocket
{
public:
    static const uint8_t OPCODE_TEXT_FRAME = 0x1;
    bool send(int client, const string& message, uint8_t opcode = OPCODE_TEXT_FRAME, bool fin = true);
};

class WebSocketClientServer
{
public:

    bool listen(uint16_t port, int domain = AF_INET);
    void start();
};

%feature("director") WebSocketCallback;
%inline %{
    class WebSocketCallback
    {
    public:
        virtual ~WebSocketCallback() {}
        virtual void run(WebSocketMessage& m, WebSocket& ws) {}
    };

    class WebSocketServer : public WebSocketClientServer
    {
    protected:
        WebSocketCallback* _callback;

    public:
        WebSocketServer() : _callback(nullptr)
        {
            EventManager::on<WebSocket::ON_MESSAGE>(Utils::to_function([this](WebSocketMessage& m, WebSocket& ws) {
                if (_callback) _callback->run(m, ws);
            }));

        }

        void on(char* event, WebSocketCallback* cb)
        {
            if (strcasecmp(event, "message") == 0) {
                if (_callback) delete _callback;
                _callback = cb;
            } else {
                exit(1);
            }
        }


    };

%}


/*********************
******* Http *********
*********************/

class HttpClientServer
{
public:
    bool listen(uint16_t port, int domain = AF_INET);
    void start();
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
};

%feature("director") HttpRequestCallback;
%inline %{
    class HttpRequestCallback
    {
    public:
        virtual ~HttpRequestCallback() {}
        virtual void run(HttpRequest& req, HttpResponse& resp) {}
    };


    class HttpServer : public HttpClientServer
    {
    protected:
        HttpRequestCallback* _callback;

    public:
        HttpServer() : _callback(nullptr)
        {
            EventManager::on<Http::ON_REQUEST>(Utils::to_function([this](HttpRequest& req, HttpResponse& resp) {
                if (_callback) _callback->run(req, resp);
            }));

        }

        void on(char* event, HttpRequestCallback* cb)
        {
            if (strcasecmp(event, "request") == 0) {
                if (_callback) delete _callback;
                _callback = cb;
            } else {
                exit(1);
            }
        }

    };
%}


/*********************
******* Socks ********
*********************/

class SocksClientServer
{
public:
    bool listen(uint16_t port, int domain = AF_INET);
    void start();
};


%inline %{
    class SocksServer : public SocksClientServer
    {
    };
%}

