#include "ReactHandler.h"

int main()
{
	ClientServer server;
	U::Echo proto(server, 1024);

	server.listen_u(8080, proto);

	server.start();

	return 0;
}
