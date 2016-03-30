#include "ReactHandler.h"
#include "Protocol/Echo.h"

int main()
{
	ClientServer server;
	ProcessWorker<Echo> worker(server);
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
