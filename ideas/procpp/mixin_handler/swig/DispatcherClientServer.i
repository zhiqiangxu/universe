template <DispatchMode m, typename Proto>
class DispatcherClientServer
{
public:
    DispatcherClientServer();

    bool listen(uint16_t port, int domain = AF_INET);

    void start();
    void daemonize(const char* std_out = "/tmp/stdout.log", const char* std_err = "/tmp/stderr.log", const char* std_in = "/dev/null");

    ssize_t write_global(uint32_t session_id, const string& data);
};



%template(SoaProcessDispatcherClientServer) DispatcherClientServer<DispatchMode::ProcessSession, Soa>;
%template(HttpProcessDispatcherClientServer) DispatcherClientServer<DispatchMode::Process, Http>;
%template(HttpGatewayProcessDispatcherClientServer) DispatcherClientServer<DispatchMode::Process, HttpGateway>;

%inline %{

    typedef DispatcherClientServer<DispatchMode::ProcessSession, Soa> SoaProcessDispatcherClientServer;
    typedef DispatcherClientServer<DispatchMode::Process, Http> HttpProcessDispatcherClientServer;
    typedef DispatcherClientServer<DispatchMode::Process, HttpGateway> HttpGatewayProcessDispatcherClientServer;

    class SoaProcessDispatcherServer : public SoaProcessDispatcherClientServer
    {
    protected:
        SoaCallback* _callback;

    public:
        SoaProcessDispatcherServer() : _callback(nullptr)
        {
            EventManager::on<Soa::ON_REQUEST>(Utils::to_function([this](SoaRequest& request, SoaResponse& response) {
                if (_callback) _callback->run(request, response);
            }));

        }

        void dispatch(int n_process)
        {
            DispatcherClientServer<DispatchMode::ProcessSession, Soa>::dispatch(n_process);
        }

        void on(char* event, SoaCallback* cb)
        {
            if (strcasecmp(event, "request") == 0) {
                if (_callback) delete _callback;
                _callback = cb;
            } else {
                exit(1);
            }
        }

        long get_session_id(int client)
        {

            uint32_t session_id;
            auto ok = ClientServer::get_session_id(client, &session_id);

            return ok ? session_id : -1;
        }

    };

    class HttpProcessDispatcherServer : public HttpProcessDispatcherClientServer
    {
    protected:
        HttpCallback* _callback;

    public:
        HttpProcessDispatcherServer() : _callback(nullptr)
        {
            EventManager::on<Http::ON_REQUEST>(Utils::to_function([this](HttpRequest& request, HttpResponse& response) {
                if (_callback) _callback->run(request, response);
            }));

        }

        void dispatch(int n_process)
        {
            DispatcherClientServer<DispatchMode::Process, Http>::dispatch(n_process);
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

    class HttpGatewayProcessDispatcherServer : public HttpGatewayProcessDispatcherClientServer
    {
    protected:
        HttpGatewayCallback* _callback;

    public:
        HttpGatewayProcessDispatcherServer() : _callback(nullptr)
        {
            EventManager::on<HttpGateway::ON_REQUEST_URI>(Utils::to_function([this](HttpRequest& request, HttpProviderAddress& target_address, bool& ok) {
                if (_callback) {
                    BoolWrapper bw(ok);
                    _callback->on_request_uri(request, target_address, bw);
                    ok = bw.ok;
                }
            }));

        }

        void dispatch(int n_process)
        {
            DispatcherClientServer<DispatchMode::Process, HttpGateway>::dispatch(n_process);
        }

        void register_callback(HttpGatewayCallback* cb)
        {
            if (_callback) delete _callback;
            _callback = cb;
        }


    };

%}
