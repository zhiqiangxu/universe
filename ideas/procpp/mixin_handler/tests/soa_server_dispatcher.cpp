#include "ReactHandler.h"

int main()
{
	ClientServer server;

    server.on<Soa::ON_REQUEST>(Utils::to_function([&server](SoaRequest& req, SoaResponse& resp) {
		resp.json = req.json;
	}));

	Dispatcher<DispatchMode::Process, Soa> dispacher(server, 4);

	auto callbacks = dispacher.to_callbacks();

	server.listen(8082, callbacks);


	server.start();

	return 0;
}
