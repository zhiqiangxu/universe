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


