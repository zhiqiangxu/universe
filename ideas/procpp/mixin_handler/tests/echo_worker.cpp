#include "ReactHandler.h"
#include "Protocol/Echo.h"

int main()
{
    ClientServer server;
    Echo proto(server);
    server.listen(8082, proto);
    server.set_worker_num(4);

    server.event_loop();

    return 0;
}
