#include "Protocol/Socks.h"
#include "ClientServer.h"
#include <arpa/inet.h>//ntohs
#include <unistd.h>//close
#include <iostream>//cout
using namespace std;

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
	_server.watch(client, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([this] (int fd, string message) {
				on_message(fd, message);
			}),
		},
		{
			EventType::CLOSE, EventManager::CB([this] (int fd) {
				on_close(fd);
			})
		}
	});

	cout << endl << "connections:" << _state.size() << endl << endl;
}

void Socks::on_message(int fd, string message)
{
	if (_buf.find(fd) != _buf.end()) {
		message = _buf[fd] + message;
		_buf.erase(fd);
	}

	cout << "message length: " << message.length() << endl;

	switch (_state[fd]) {
		case SocksState::GREETING:
		{
			if (need_buf(fd, message, message.length() < 2)) return;

			if (message[0] != 0x05) {
				cout << "not 0x50" << endl;
				close(fd);
				return;
			}
			auto num_methods = static_cast<int>(message[1]);

			if (need_buf(fd, message, message.length() < static_cast<size_t>(2 + num_methods))) return;

			bool no_auth = false;
			cout << "methods: " << num_methods << " ( ";
			for(auto i = 0; i < num_methods; i++) {
				cout << static_cast<int>(message[2+i]) << " ";
				if (message[2+i] == 0) {
					no_auth = true;
					//break; //for log
				}
			}
			cout << ")" << endl;

			if (!no_auth) {
				cout << "not no_auth" << endl;
				close(fd);
				return;
			}

			string response("\x05\x0", 2);
			write(fd, response.data(), response.length());

			_state[fd] = SocksState::REQUEST;

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

			size_t length = 4 + 2/*port*/;
			switch (message[3]) {
				case 0x01:
					length += 4;//4 bytes for IPv4
					break;
				case 0x03:
					length += 1 + message[4];//1 byte of name length followed by the name for Domain name
					break;
				case 0x04:
					length += 16;//16 bytes for IPv6 address
					break;
				default:
					cout << "invalid address type" << endl;
					close(fd);
					return;
			}

			cout << "required length: " << length << endl;

			if (need_buf(fd, message, message.length() < length)) return;

			const uint16_t port = ntohs(*reinterpret_cast<const uint16_t*>(message.substr(length - 2, 2).c_str()));
			switch (message[3]) {
				case 0x01:
				{
					cout << "address type 0x01 not impl, terminate" << endl;
					close(fd);
					return;
				}
				case 0x03:
				{
					auto address = message.substr(5, message[4]);
					cout << "connect remote:\t" << address << ":" << port << endl;
					if (!dynamic_cast<ClientServer&>(_server).connect(address, port, EventManager::EventCB{
						{
							EventType::CONNECT, EventManager::CB([fd,this] (int remote_fd) {
								_c2r[fd] = remote_fd;
								_r2c[remote_fd] = fd;
							})
						},
						{
							EventType::READ, EventManager::CB([fd, this] (int remote_fd, string message) {
								send_peer(fd, message);
							})
						},
						{
							EventType::CLOSE, EventManager::CB([fd, this] (int remote_fd) {
								close(fd);
							})
						}
					})) {
						cout << "connect remote fail:" << address << endl;
						close(fd);
					}

					cout << "suceeded: " << fd << endl;
					string response("\x05\x00\x00\x03", 4);
					response.append(1, message[4]);
					response.append(address);
					response.append(string(reinterpret_cast<const char*>(&port), sizeof(port)));
					write(fd, response.data(), response.length());
					cout << "response length: " << response.length() << endl;

					_state[fd] = SocksState::CONNECT;

					if (message.length() > length) {
						cout << "trigger next frame" << endl;
						on_message(fd, message.substr(length));
					}

					break;
				}
				case 0x04:
				{
					cout << "address type 0x04 not impl, terminate" << endl;
					close(fd);
					return;
				}
			}

			break;
		}
		case SocksState::CONNECT:
		{
			cout << "send_peer" << endl;
			send_peer(_c2r[fd], message);
			break;
		}
	}

}

bool Socks::send_peer(int peer_fd, string& message)
{
	return write(peer_fd, message.data(), message.length());
}

void Socks::on_close(int fd)
{
	cout << "[on_close] " << fd << endl;
	if (_c2r.find(fd) != _c2r.end()) {
		auto remote_fd = _c2r[fd];
		_server.remove(remote_fd);
		::close(remote_fd);
		_c2r.erase(fd);
		_r2c.erase(remote_fd);
	}
	_state.erase(fd);
	_buf.erase(fd);
	_server.remove(fd);

	cout << endl << "connections:" << _state.size() << endl << endl;
}

void Socks::close(int fd)
{
	cout << "[close] " << fd << endl;
	::close(fd);
	on_close(fd);
}

bool Socks::need_buf(int fd, string& message, bool cond)
{
	if (cond) {
		_buf[fd] = message;
		return true;
	}

	return false;
}
