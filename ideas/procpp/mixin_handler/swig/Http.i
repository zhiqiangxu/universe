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

%feature("director") HttpCallback;
%inline %{
    class HttpCallback
    {
    public:
        virtual ~HttpCallback() {}
        virtual void run(HttpRequest& req, HttpResponse& resp) {}
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


