#include "Server.h"
#include "Protocol/Echo.h"




int main()
{
	Server server;
	Echo proto(server);
	server.listen(8081, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([&proto] (int fd) {
				proto.handle(fd);
			})
		}
	});
	
	server.start();

	return 0;
}
