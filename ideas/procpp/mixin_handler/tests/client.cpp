#include "ReactHandler.h"
#include <unistd.h>
#include <string.h>
#include <iostream>

int main()
{
	Client client;


	client.connect("localhost", 8082, EventManager::EventCB{
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
			EventType::CLOSE, EventManager::CB([&client] (int fd) {
				cout << "[closed]" << endl;

				/*get_peer_name will fail after closed by peer*/
				Utils::SocketAddress addr;
				if (Utils::get_peer_name(fd, addr)) {
					cout << "getpeername ok" << endl;
				} else {
					L.error_log("getpeername ng");
				}

			})
		}

	});

	client.start();
	return 0;
}
