#include "ReactHandler.h"





int main()
{
    ClientServer server;
    server.set_parent("localhost", 8081);

    ::Echo proto(server);
    server.listen(8079, proto);


    server.event_loop();

    return 0;
}
