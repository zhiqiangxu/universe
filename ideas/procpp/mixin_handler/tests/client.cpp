#include "Client.h"
#include <unistd.h>
#include <string.h>
#include <iostream>

int main()
{
	Client client;

	client.connect("localhost", 8081, EventManager::EventCB{
		{
			EventType::CONNECT, EventManager::CB([] (int fd, ConnectResult r) {
				cout << "[connect]" << endl;
				write(fd, "connect", strlen("connect"));
			})
		},
		{
			EventType::READ, EventManager::CB([] (int fd, string message) {
				cout << "[read]" << message.length() << endl;
				cout << message << endl;
				write(fd, "hello", strlen("hello"));
				sleep(2);
			})
		},
		{
			EventType::CLOSE, EventManager::CB([] (int fd) {
				cout << "[closed]" << endl;
			})
		}

	});

	client.start();
	return 0;
}
