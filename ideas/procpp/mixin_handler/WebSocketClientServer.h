#pragma once
#include "ClientServer.h"
#include "Protocol/WebSocket.h"

class IWebSocketClientServer
{
public:
};

class WebSocketClientServer : public ClientServer, public IWebSocketClientServer
{
private:
    WebSocket _proto;
public:

    WebSocketClientServer(uint16_t port, int domain = AF_INET);
};
