#pragma once

#include "ClientServer.h"
#include "Protocol/Socks.h"

class ISocksClientServer
{
public:
    virtual bool listen(uint16_t port, int domain) = 0;
};

class SocksClientServer : public ClientServer, public ISocksClientServer
{
private:
    Socks _proto;
public:
    SocksClientServer() : _proto(*this) {}
    virtual bool listen(uint16_t port, int domain = AF_INET) override;
};
