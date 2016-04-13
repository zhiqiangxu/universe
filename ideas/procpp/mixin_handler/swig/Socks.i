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


