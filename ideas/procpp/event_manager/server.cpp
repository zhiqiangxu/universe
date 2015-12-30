#include "EventManager.h"
#include <iostream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include "Server.h"


using namespace std;

void error_exit(const char *s)
{
	perror(s);
	exit(1);
}


int main()
{
	Server server;
	server.listen(8081, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([&server] (int fd) {
				auto client = accept(fd, nullptr, nullptr);
				if (client == -1) error_exit("accept");

				server.getEventManager().watch(client, EventManager::EventCB{
					{
						EventType::READ, EventManager::CB([] (int fd, string message) {
							cout << "[read]" << endl;
							cout << message;
							write(fd, message.data(), message.length());
						}),
					},
					{
						EventType::CLOSE, EventManager::CB([] (int fd) {
							cout << "[closed]" << endl;
						})
					}
				});
			})
		}
	});
	
	server.start();

	return 0;
}
