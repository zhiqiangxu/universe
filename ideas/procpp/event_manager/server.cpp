#include "EventManager.h"
#include <iostream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>


using namespace std;

void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

int server_socket_ip4(uint16_t port)
{
	auto s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) error_exit("socket");

	const int enable = 1;
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) error_exit("setsockopt");
	
	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	// bind,listen接受的都是sockaddr类型参数
	if (bind(s, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) error_exit("bind");

	if (listen(s, 100) < 0) error_exit("listen");

	cout << "listening port " << port << endl;
	return s;
}


int main()
{
	EventManager m;
	auto s = server_socket_ip4(8081);
	m.watch(s, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([s, &m] (int fd) {
				auto client = accept(s, nullptr, nullptr);
				if (client == -1) error_exit("accept");

				m.watch(client, EventManager::EventCB{
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
	
	m.start();
	return 0;
}
