#pragma once

#include "ClientServer.h"
#include "Protocol/Http.h"

class IHttpClientServer
{
public:
    virtual bool listen(uint16_t port, int domain) = 0;
};

class HttpClientServer : public ClientServer, public IHttpClientServer
{
private:
    Http _proto;
public:
    HttpClientServer() : _proto(*this) {}
    virtual bool listen(uint16_t port, int domain = AF_INET) override;
};
