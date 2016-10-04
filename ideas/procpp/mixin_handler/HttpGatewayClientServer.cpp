#include "ReactHandler.h"

HttpGatewayClientServer::HttpGatewayClientServer(uint16_t port, int domain): _proto(*this)
{
    listen(port, _proto, domain);
}


