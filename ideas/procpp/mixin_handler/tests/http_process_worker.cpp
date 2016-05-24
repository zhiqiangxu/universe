#include "ReactHandler.h"

int main()
{
	ClientServer server;
    Http proto(server);

    server.on<Http::ON_REQUEST>(Utils::to_function([&server](HttpRequest& req, HttpResponse& resp) {
		resp.body = "content from c++\r\n";
	}));

	server.listen(8082, proto);
	
	ProcessWorker worker(server);

	return 0;
}
