#include "ReactHandler.h"

int main()
{
	ClientServer server;

	server.listen_u(8080, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([] (int fd) {
				cout << "read event" << endl;
			})
		}
	});

	server.start();

	return 0;
}
