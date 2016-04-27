#pragma once

#include "ClientServer.h"
#include "Protocol/Soa.h"

class ISoaClientServer
{
public:
    virtual bool listen(uint16_t port, int domain) = 0;
};

class SoaClientServer : public ClientServer, public ISoaClientServer
{
private:
    Soa _proto;
public:
    SoaClientServer() : _proto(*this) {}
    virtual bool listen(uint16_t port, int domain = AF_INET) override;
};
