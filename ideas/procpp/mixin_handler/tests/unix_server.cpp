#include "ReactHandler.h"
#include "Protocol/Echo.h"

int main()
{
    ClientServer worker_server;
    Echo proto(worker_server);

    auto sockaddr = Utils::addr_sun("/tmp/test.sock");

    worker_server.listen(reinterpret_cast<const struct sockaddr*>(&sockaddr), sizeof(sockaddr), proto);

    worker_server.event_loop();

    return 0;
}

