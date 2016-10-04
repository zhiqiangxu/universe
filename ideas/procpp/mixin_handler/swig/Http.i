class HttpClientServer : public ClientServer
{
public:
    HttpClientServer(uint16_t port, int domain = AF_INET);
};

class HttpGatewayClientServer : public ClientServer
{
public:

    HttpGatewayClientServer(uint16_t port, int domain = AF_INET);
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
    string reason_phrase = "OK";
    vector<pair<string, string>> headers;
    string body;
};

class HttpProviderAddress
{
public:
    string host;
    int port;
    string path;
    string query;
    bool ssl = false;
};

%feature("director") HttpCallback;
%feature("director") HttpGatewayCallback;
%inline %{

    /*TODO remove wrapper..*/
    class BoolWrapper
    {
    public:
        BoolWrapper(bool v) : ok(v) {}
        bool ok;
    };

    class HttpCallback
    {
    public:
        virtual ~HttpCallback() {}
        virtual void run(HttpRequest& req, HttpResponse& resp) {}
    };
    class HttpGatewayCallback
    {
    public:
        virtual ~HttpGatewayCallback() {}
        virtual void on_request_uri(HttpRequest& req, HttpProviderAddress& target_address, BoolWrapper& ok) {}
        virtual void on_response(HttpRequest& request, HttpResponse& response) {}
    };


    class HttpServer : public HttpClientServer
    {
    protected:
        HttpCallback* _callback;

    public:
        HttpServer(uint16_t port, int domain = AF_INET) : HttpClientServer(port, domain), _callback(nullptr)
        {
            HttpClientServer::on<Http::ON_REQUEST>(Utils::to_function([this](HttpRequest& req, HttpResponse& resp) {
                if (_callback) _callback->run(req, resp);
            }));

        }

        void on(char* event, HttpCallback* cb)
        {
            if (strcasecmp(event, "request") == 0) {
                if (_callback) delete _callback;
                _callback = cb;
            } else {
                exit(1);
            }
        }

    };

    class HttpGatewayServer : public HttpGatewayClientServer
    {

    protected:
        HttpGatewayCallback* _callback;

    public:
        HttpGatewayServer(uint16_t port, int domain = AF_INET) : HttpGatewayClientServer(port, domain), _callback(nullptr)
        {
            this->on<HttpGateway::ON_REQUEST_URI>(Utils::to_function([this](HttpRequest& request, HttpProviderAddress& target_address, bool& ok) {
                if (_callback) {
                    BoolWrapper bw(ok);
                    _callback->on_request_uri(request, target_address, bw);
                    ok = bw.ok;
                }
            }));
            this->on<HttpGateway::ON_RESPONSE>(Utils::to_function([this](HttpRequest& request, HttpResponse& response) {
                if (_callback) {
                    _callback->on_response(request, response);
                }
            }));

        }

        void register_callback(HttpGatewayCallback* cb)
        {
            if (_callback) delete _callback;
            _callback = cb;
        }


    };

%}


