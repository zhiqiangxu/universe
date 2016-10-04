#pragma once

#include "ClientServer.h"
#include "Protocol/Soa.h"

class ISoaClientServer
{
public:
};

class SoaClientServer : public ClientServer, public ISoaClientServer
{
private:
    Soa _proto;
public:
    SoaClientServer(uint16_t port, int domain = AF_INET);
};
