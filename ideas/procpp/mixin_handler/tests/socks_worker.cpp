#include "ReactHandler.h"
#include "Protocol/Socks.h"

int main()
{
	ClientServer server;
	ProcessSessionWorker<Socks> worker(server);
	server.listen(8082, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([&worker] (int fd) {
				worker.handle(fd);
			})
		}
	});
	
	server.start();

	return 0;
}
