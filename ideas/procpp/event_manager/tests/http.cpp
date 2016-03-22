#include "ReactHandler.h"

int main()
{
	ClientServer server;
	Http proto(server);
	server.listen(8082, proto);
	server.start();

	return 0;
}
