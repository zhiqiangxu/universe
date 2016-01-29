#include "ReactHandler.h"

int main()
{
	ClientServer server;
	U::Echo uproto(server);

	server.listen_u(8080, uproto);

	server.start();

	return 0;
}
