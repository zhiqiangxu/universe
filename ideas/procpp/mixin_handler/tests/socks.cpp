#include "ReactHandler.h"

int main()
{
	SocksClientServer server;
	server.listen(8082);
	server.start();

	return 0;
}
