#pragma once

#include "ClientServer.h"
#include "Protocol/Http.h"

class IHttpClientServer
{
public:
};

class HttpClientServer : public ClientServer, public IHttpClientServer
{
private:
    Http _proto;
public:
    HttpClientServer(uint16_t port, int domain = AF_INET);
};
