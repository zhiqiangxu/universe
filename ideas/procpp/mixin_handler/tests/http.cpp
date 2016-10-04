#include "ReactHandler.h"

int main()
{
    HttpClientServer server(8082);

    server.on<Http::ON_REQUEST>(Utils::to_function([&server](HttpRequest& req, HttpResponse& resp) {
        resp.body = "content from c++\r\n";
    }));

    server.event_loop();

    return 0;
}
