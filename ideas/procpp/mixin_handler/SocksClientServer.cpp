#include "ReactHandler.h"

SocksClientServer::SocksClientServer(uint16_t port, int domain): _proto(*this)
{
    listen(port, _proto, domain);
}
