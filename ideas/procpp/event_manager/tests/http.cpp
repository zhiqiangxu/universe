#include "ReactHandler.h"

int main()
{
	ClientServer server;
	Http proto(server);

	using Hook = EventHookGlobal<Http::ON_REQUEST, HttpRequest&, HttpResponse&>;
	auto id = Hook::get_instance().attach([&server](HttpRequest& req, HttpResponse& resp) {
		resp.body = "additional content from hook\r\n";
	});


	server.listen(8082, proto);
	server.start();

	return 0;
}
