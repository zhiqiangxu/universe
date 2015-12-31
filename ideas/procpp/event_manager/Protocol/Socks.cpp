#include "Protocol/Socks.h"
#include <unistd.h>//close

static void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

void Socks::handle(int fd)
{
	auto client = accept(fd, nullptr, nullptr);
	if (client == -1) error_exit("accept");

	_state[client] = SocksState::GREETING;
	_server._m.watch(client, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([&this] (int fd, string message) {
				this->on_message(fd, message);
			}),
		},
		{
			EventType::CLOSE, EventManager::CB([&this] (int fd) {
				this->on_close(fd);
				cout << "[closed]" << endl;
			})
		}
	});
}

void Socks::on_message(int fd, string message)
{
	if (_buf.find(fd) != _buf.end()) {
		message = _buf[fd] + message;
		_buf.erase(fd);
	}

	switch (_state[fd]) {
		case SocksState::GREETING:
		{
			if (need_buf(fd, message, message.length() < 2)) return;

			if (message[0] != 0x05) {
				cout << "not 0x50" << endl;
				close(fd);
				return;
			}
			auto num_methods = message[1];

			if (need_buf(fd, message, message.length() < 2 + num_methods)) return;

			bool no_auth = false;
			for(auto i = 0; i < num_methods; i++) {
				if (message[2+i] == 0) {
					no_auth = true;
					break;
				}
			}

			if (!no_auth) {
				cout << "not no_auth" << endl;
				close(fd);
				return;
			}

			const char response[] = [0x05, 0];
			write(fd, response, sizeof(response));

			_state[client] = SocksState::REQUEST;

			break;
		}
		case SocksState::REQUEST:
		{
			if (need_buf(fd, message, message.length() < 5)) return;

			if (message[1] != 0x01) {
				cout << "not connect" << endl;
				close(fd);
				return;
			}

			auto addrlen = message[4];

			if (need_buf(fd, message, message.length() < 5 + addrlen + 2)) return;

			auto addr = message.substr(5, addrlen);
			break;
		}
		case SocksState::CONNECT:
		{
			break;
		}
	}

}

void Socks::on_close(int fd)
{
	cout << "[on_close]" << endl;
	_state.erase(fd);
	_buf.erase(fd);
}

void Socks::close(int fd)
{
	::close(fd);
	on_close(fd);
}

void Socks::need_buf(int fd, string& message, bool cond)
{
	if (cond) {
		_buf[fd] = message;
		return true;
	}

	return false;
}
