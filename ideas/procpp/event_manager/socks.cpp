#include "ClientServer.h"
#include "Protocol/Socks.h"

int main()
{
	ClientServer server;
	Socks proto(server);
	server.listen(8082, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([&proto] (int fd) {
				proto.handle(fd);
			})
		}
	});
	
	server.start();

	return 0;
}
