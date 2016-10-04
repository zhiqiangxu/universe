#include "ReactHandler.h"




int main()
{
    Server server;
    Echo proto(server);
    server.listen(8081, proto);

    server.schedule_once([] () { cout << "schedule_once output" << endl;}, 3);
    server.schedule([] () { cout << "schedule output" << endl;}, 0, 1);


    server.event_loop();

    return 0;
}
