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
    };


    class HttpServer : public HttpClientServer
    {
    protected:
        HttpCallback* _callback;

    public:
        HttpServer() : _callback(nullptr)
        {
            EventManager::on<Http::ON_REQUEST>(Utils::to_function([this](HttpRequest& req, HttpResponse& resp) {
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
%}


