#include "Protocol/Echo.h"
#include <iostream>//cout,endl
#include <unistd.h>//write
using namespace std;

static void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

void Echo::handle(int fd)
{
	auto client = accept(fd, nullptr, nullptr);
	if (client == -1) error_exit("accept");

	_server.watch(client, EventManager::EventCB{
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
}
