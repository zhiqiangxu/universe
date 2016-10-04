#include "ReactHandler.h"

int main()
{
    ClientServer server;

    server.on<Soa::ON_REQUEST>(Utils::to_function([&server](SoaRequest& req, SoaResponse& resp) {
        resp.json = req.json;
    }));

    Soa proto(server);
    //server.set_worker_num(4);

    server.listen(8082, proto);


    server.event_loop();
}
