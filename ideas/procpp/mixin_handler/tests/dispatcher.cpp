#include "ReactHandler.h"

int main()
{
	ClientServer server;
    server.on<Http::ON_REQUEST>(Utils::to_function([&server](HttpRequest& req, HttpResponse& resp) {
		resp.body = "content from c++\r\n";
	}));


	Dispatcher<DispatchMode::ProcessSession, Http> dispacher(server, 2);

	auto callbacks = dispacher.to_callbacks();

	server.listen(8082, callbacks);


	server.start();

	return 0;
}
