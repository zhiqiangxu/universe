#include "EventManager.h"
#include <iostream>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>


using namespace std;

void error_exit(string s)
{
	cout << "error:\t" << s << endl;
	exit(1);
}

int server_socket_ip4(uint16_t port)
{
	auto s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) error_exit("socket");
	
	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	// bind,listen接受的都是sockaddr类型参数
	if (bind(s, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) error_exit("bind");

	if (listen(s, 100) < 0) error_exit("listen");

	return s;
}

string _read_fd(int fd)
{
	string message("");
	char buf[1024];

	while (true) {
		auto size = read(fd, buf, sizeof(buf));
		cout << "size=" << size << endl;
		if (size == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return message;
			} else {
				//error handle
			}
			continue;
		} else if (size == 0) {
			//eof
			break;
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
	auto s = server_socket_ip4(8081);
	m.watch(s, EventManager::EventCB{
		{
			EventType::READ, [s, &m] (int fd) {
				auto client = accept(s, nullptr, nullptr);
				if (client == -1) error_exit("accept");

				m.watch(client, EventManager::EventCB{
					{
						EventType::READ, [] (int fd) {
							cout << "read" << endl;
							cout << _read_fd(fd);
						},
					},
					{
						EventType::CLOSE, [] (int fd) {
							cout << "closed" << endl;
						}
					}
				});
			}
		}
	});
	
	m.start();
	return 0;
}
