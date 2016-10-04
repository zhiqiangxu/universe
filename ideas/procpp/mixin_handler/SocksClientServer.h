#pragma once

#include "ClientServer.h"
#include "Protocol/Socks.h"

class ISocksClientServer
{
public:
};

class SocksClientServer : public ClientServer, public ISocksClientServer
{
private:
    Socks _proto;
public:
    SocksClientServer(uint16_t port, int domain = AF_INET);
};
