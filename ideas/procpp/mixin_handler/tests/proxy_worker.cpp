#include "ReactHandler.h"


int main()
{
	ClientServer server;

	auto addr_list = { Utils::to_addr(/*"localhost"*/"127.0.0.1", 6379) };
	ProxyWorker worker(server, addr_list);

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
