#include "ReactHandler.h"

bool SoaClientServer::listen(uint16_t port, int domain)
{
    return ClientServer::listen(port, _proto, domain);
}
