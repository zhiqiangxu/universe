#include "EventManager.h"
#include <iostream>
#include <unistd.h>
using namespace std;


int main()
{
	EventManager m;
	if (m.watch(STDIN_FILENO, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([] (int fd, string message) {
				cout << message;
			})
		}
	})) {
		cout << "watch ok" << endl;
	} else {
		cout << "watch ng" << endl;
	}

	m.start();

	return 0;
}
