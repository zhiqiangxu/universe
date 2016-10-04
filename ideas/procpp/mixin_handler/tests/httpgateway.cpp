#include "ReactHandler.h"

int main()
{
    ClientServer server;
    HttpGateway proto(server);

    server.on<HttpGateway::ON_REQUEST_URI>(Utils::to_function([&server](HttpRequest& req, HttpProviderAddress& target_address, bool& ok) {
        ok = true;
        target_address.host = "blog.csdn.net";
        target_address.port = 80;
        target_address.path = "/jasonliuvip/article/details/23567843";
    }));

    server.listen(8082, proto);
    server.event_loop();

    return 0;
}
