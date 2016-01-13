#include "Protocol/Echo.h"
#include <iostream>//cout,endl
#include <unistd.h>//write
#include <string>
using namespace std;

static void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

void Echo::handle(int fd)
{
	auto client = _server.accept(fd, nullptr, nullptr);
	if (client == -1) error_exit("accept");
	cout << "[client " + to_string(client) + "]" << endl;

	_server.watch(client, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([] (int fd, string message) {
				cout << "[read " + to_string(fd) + "]" << endl;
				cout << message;
				write(fd, message.data(), message.length());
			}),
		},
		{
			EventType::CLOSE, EventManager::CB([] (int fd) {
				cout << "[closed " + to_string(fd) + "]" << endl;
			})
		}
	});
}
