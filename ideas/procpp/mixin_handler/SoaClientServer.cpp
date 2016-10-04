#include "ReactHandler.h"

SoaClientServer::SoaClientServer(uint16_t port, int domain): _proto(*this)
{
    listen(port, _proto, domain);
}
