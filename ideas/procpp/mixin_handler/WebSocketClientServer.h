#pragma once
#include "ClientServer.h"
#include "Protocol/WebSocket.h"

class IWebSocketClientServer
{
public:
    virtual bool listen(uint16_t port, int domain) = 0;
};

class WebSocketClientServer : public ClientServer, public IWebSocketClientServer
{
private:
    WebSocket _proto;
public:
    WebSocketClientServer() : _proto(*this) {}
    virtual bool listen(uint16_t port, int domain = AF_INET) override;
};
