#include "ReactHandler.h"

WebSocketClientServer::WebSocketClientServer(uint16_t port, int domain): _proto(*this)
{
    listen(port, _proto, domain);
}
