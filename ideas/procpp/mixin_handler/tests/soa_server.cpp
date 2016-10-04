#include "ReactHandler.h"

int main()
{
    SoaClientServer server(8082);

    server.on<Soa::ON_REQUEST>(Utils::to_function([&server](SoaRequest& req, SoaResponse& resp) {
        resp.json = req.json;
    }));

    server.event_loop();

    return 0;
}
