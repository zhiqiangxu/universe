#include "ReactHandler.h"

int main()
{
// old style
/*
	ClientServer server;

    server.on<Soa::ON_REQUEST>(Utils::to_function([&server](SoaRequest& req, SoaResponse& resp) {
		resp.json = req.json;
	}));

	Dispatcher<DispatchMode::ProcessSession, Soa> dispacher(server, 4);

	auto callbacks = dispacher.to_callbacks();

	server.listen(8082, callbacks);


	server.start();
*/

// new style

    DispatcherClientServer<DispatchMode::ProcessSession, Soa> server;
    server.on<Soa::ON_REQUEST>(Utils::to_function([](SoaRequest& req, SoaResponse& resp) {
		resp.json = req.json;
	}));

    server.dispatch(4);
	server.listen(8082);
    server.start();
	return 0;
}
