#include "ReactHandler.h"

int main()
{
	ClientServer server;
	Http proto(server);

    server.on<Http::ON_REQUEST>(Utils::to_function([&server](HttpRequest& req, HttpResponse& resp) {
		resp.body = "additional content from hook\r\n";
	}));

	server.listen(8082, proto);
	server.start();

	return 0;
}
