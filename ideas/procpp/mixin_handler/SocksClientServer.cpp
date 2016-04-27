#include "ReactHandler.h"

bool SocksClientServer::listen(uint16_t port, int domain)
{
    return ClientServer::listen(port, _proto, domain);
}
