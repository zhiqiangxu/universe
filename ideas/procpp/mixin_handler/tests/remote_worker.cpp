#include "ReactHandler.h"


int main()
{
	ClientServer server;

	auto addr_list = { Utils::to_addr("localhost"/*"127.0.0.1"*/, 8079) };

	RemoteWorker worker(server, addr_list, 8081);

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
