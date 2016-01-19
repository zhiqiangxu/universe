#include "Server.h"
#include "Protocol/Echo.h"




int main()
{
	Server server;
	Echo proto(server);
	server.listen(8081, proto);
	server.start();

	return 0;
}
