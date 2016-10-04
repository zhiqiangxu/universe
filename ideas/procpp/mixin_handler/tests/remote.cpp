#include "ReactHandler.h"


int main()
{
    ClientServer server;

    auto addr_list = { Utils::to_addr("localhost"/*"127.0.0.1"*/, 8079) };

    Remote proto(server, addr_list, 8081);

    server.listen(8082, proto);
    
    server.event_loop();

    return 0;
}
