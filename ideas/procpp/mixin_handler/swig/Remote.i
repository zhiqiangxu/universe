class Remote : public Protocol/* required so that swig can recognize Proxy as derived class */
{
public:

    Remote(ClientServer& server, const vector<Utils::SocketAddress>& l, uint16_t port);

};


