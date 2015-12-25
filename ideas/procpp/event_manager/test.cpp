#include "EventManager.h"
#include <iostream>
#include <unistd.h>
using namespace std;

string _read_fd(int fd)
{
	string message("");
	char buf[1024];

	while (true) {
		auto size = read(fd, buf, sizeof(buf));
		cout << "size:" << size;
		if (size == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return message;
			} else {
				//error handle
			}
			continue;
		}

		// TODO 避免拷贝
		string sbuf("");
		sbuf.assign(buf, size);
		message += sbuf;
	}

	return message;
}

int main()
{
	EventManager m;
	if (m.watch(fileno(stdin), EventType::READ, [] (int fd) {
		cout << "callback start: " << fd << endl;
		cout << _read_fd(fd) << endl;
		cout << "callback end" << endl;
	})) {
		cout << "watch ok" << endl;
	} else {
		cout << "watch ng" << endl;
	}

	m.start();

	return 0;
}
