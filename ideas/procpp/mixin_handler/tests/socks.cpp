#include "ClientServer.h"
#include "Protocol/Socks.h"

int main()
{
	ClientServer server;
	Socks proto(server);
	server.listen(8082, proto);
	server.start();

	return 0;
}
