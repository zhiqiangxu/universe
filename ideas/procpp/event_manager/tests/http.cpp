#include "ReactHandler.h"

int main()
{
	ClientServer server;
	Http proto(server);

	using Hook = EventHookGlobal<Http::ON_REQUEST, HttpRequest&, int>;
	auto id = Hook::get_instance().attach([&server](HttpRequest& r, int client) {
		server.write(client, "additional content from hook\r\n", sizeof("additional content from hook\r\n"));
	});


	server.listen(8082, proto);
	server.start();

	return 0;
}
