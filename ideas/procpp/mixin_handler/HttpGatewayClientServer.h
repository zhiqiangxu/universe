#pragma once

#include "ClientServer.h"
#include "Protocol/HttpGateway.h"

class IHttpGatewayClientServer
{
public:
};

class HttpGatewayClientServer : public ClientServer, public IHttpGatewayClientServer
{
private:
    HttpGateway _proto;
public:
    HttpGatewayClientServer(uint16_t port, int domain = AF_INET);
};
