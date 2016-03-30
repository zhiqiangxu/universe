#include "ReactHandler.h"
#include "Protocol/EasyP2P.h"

int main()
{
	Server server;
	EasyP2P proto(server);
	server.listen(8081, proto);
	server.start();

	return 0;
}
