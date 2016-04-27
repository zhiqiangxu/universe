#include "ReactHandler.h"

int main()
{
	SoaClientServer server;

    server.on<Soa::ON_REQUEST>(Utils::to_function([&server](SoaRequest& req, SoaResponse& resp) {
		resp.json = req.json;
	}));

	server.listen(8082);
	server.start();

	return 0;
}
