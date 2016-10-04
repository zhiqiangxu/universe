class SocksClientServer
{
public:

    SocksClientServer(uint16_t port, int domain = AF_INET);
};


%inline %{
    class SocksServer : public SocksClientServer
    {
    };
%}


