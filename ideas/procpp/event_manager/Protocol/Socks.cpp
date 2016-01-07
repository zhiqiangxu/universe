#include "Protocol/Socks.h"
#include "ClientServer.h"
#include <arpa/inet.h>//ntohs
#include <unistd.h>//write
#include <iostream>//cout
using namespace std;

//https://en.wikipedia.org/wiki/ANSI_escape_code
static string RED(string s)
{
	return "\033[1;31m" + s + "\033[0m";
}
static string GREEN(string s)
{
	return "\033[1;32m" + s + "\033[0m";
}
static string YELLOW(string s)
{
	return "\033[1;33m" + s + "\033[0m";
}
static void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

void Socks::handle(int fd)
{
	while (true) {
		auto client = accept(fd, nullptr, nullptr);
		if (client == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				cout << "[accept]" << endl;
				cout << endl << GREEN("connections:" + to_string(_state.size()) + " sockets:" + to_string(_server.count())) << endl << endl;
				return;
			}

			error_exit("accept");
		}

		cout << "[client] " << GREEN(to_string(client)) << endl;

		_state[client] = SocksState::GREETING;
		_server.watch(client, EventManager::EventCB{
			{
				EventType::READ, EventManager::CB([this] (int client, string message) {
					on_message(client, message);
				}),
			},
			{
				EventType::CLOSE, EventManager::CB([this] (int client) {
					on_close(client);
				})
			}
		});

	}
}

void Socks::on_message(int client, string message)
{
	if (_buf.find(client) != _buf.end()) {
		message = _buf[client] + message;
		_buf.erase(client);
	}

	//cout << "message length: " << message.length() << endl;

	switch (_state[client]) {
		case SocksState::GREETING:
		{
			if (need_buf(client, message, message.length() < 2)) return;

			if (message[0] != 0x05) {
				cout << RED("not 0x50") << endl;
				close(client);
				return;
			}
			auto num_methods = static_cast<int>(message[1]);

			if (need_buf(client, message, message.length() < static_cast<size_t>(2 + num_methods))) return;

			bool no_auth = false;
			//cout << "methods: " << num_methods << " ( ";
			for(auto i = 0; i < num_methods; i++) {
				//cout << static_cast<int>(message[2+i]) << " ";
				if (message[2+i] == 0) {
					no_auth = true;
					//break; //for log
				}
			}
			//cout << ")" << endl;

			if (!no_auth) {
				cout << RED("not no_auth") << endl;
				close(client);
				return;
			}

			string response("\x05\x0", 2);
			write(client, response.data(), response.length());

			_state[client] = SocksState::REQUEST;

			break;
		}
		case SocksState::REQUEST:
		{
			if (need_buf(client, message, message.length() < 5)) return;

			if (message[1] != 0x01) {
				cout << RED("not connect") << endl;
				close(client);
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
					cout << RED("invalid address type") << endl;
					close(client);
					return;
			}

			//cout << "required length: " << length << endl;

			if (need_buf(client, message, message.length() < length)) return;

			const uint16_t port = ntohs(*reinterpret_cast<const uint16_t*>(message.substr(length - 2, 2).c_str()));
			switch (message[3]) {
				case 0x01:
				{
					cout << RED("address type 0x01 not impl, terminate") << endl;
					close(client);
					return;
				}
				case 0x03:
				{
					auto address = message.substr(5, message[4]);
					cout << "connect remote:\t" << address << ":" << port << endl;
					if (!dynamic_cast<ClientServer&>(_server).connect(address, port, EventManager::EventCB{
						{
							EventType::CONNECT, EventManager::CB([client,this] (int remote_fd) {
								_c2r[client] = remote_fd;
								_r2c[remote_fd] = client;
							})
						},
						{
							EventType::READ, EventManager::CB([client, this] (int remote_fd, string message) {
								send_peer(client, message);
							})
						},
						{
							EventType::CLOSE, EventManager::CB([client, this] (int remote_fd) {
								cout << YELLOW("remote_fd should be " + to_string(remote_fd)) << endl;
								close(client);
							})
						}
					})) {
						cout << RED("connect remote fail:" + address) << endl;
						close(client);
						return;
					}

					cout << GREEN("connect ok  " + address) << endl;
					cout << endl << GREEN("connections:" + to_string(_state.size()) + " sockets:" + to_string(_server.count())) << endl << endl;

					//cout << "suceeded: " << client << endl;
					string response("\x05\x00\x00\x03", 4);
					response.append(1, message[4]);
					response.append(address);
					response.append(string(reinterpret_cast<const char*>(&port), sizeof(port)));
					write(client, response.data(), response.length());
					//cout << "response length: " << response.length() << endl;

					_url[client] = address;
					_state[client] = SocksState::CONNECT;

					if (message.length() > length) {
						cout << RED("trigger next frame") << endl;
						on_message(client, message.substr(length));
					}

					break;
				}
				case 0x04:
				{
					cout << RED("address type 0x04 not impl, terminate") << endl;
					close(client);
					return;
				}
			}

			break;
		}
		case SocksState::CONNECT:
		{
			//cout << "send_peer length: " << message.length()  << endl;
			send_peer(_c2r[client], message);
			break;
		}
	}

}

bool Socks::send_peer(int peer_fd, string& message)
{
	return write(peer_fd, message.data(), message.length());
}

void Socks::on_close(int client)
{
	auto address = _url.find(client) != _url.end() ? _url[client] : "";

	cout << "[on_close] " << GREEN(to_string(client) + " client " + address) << endl;
	if (_state.find(client) == _state.end()) {
		cout << "already closed" << endl;
		cout << endl << GREEN("connections:" + to_string(_state.size()) + " sockets:" + to_string(_server.count())) << endl << endl;
		return;//已关闭
	}

	_state.erase(client);
	_buf.erase(client);
	_url.erase(client);

	if (_c2r.find(client) == _c2r.end()) return;


	auto remote_fd = _c2r[client];

	cout << YELLOW("remote_fd is " + to_string(remote_fd)) << endl;

	_c2r.erase(client);
	_r2c.erase(remote_fd);

	cout << (_server.close(remote_fd) ? GREEN("OK1") : RED("NG1")) << endl;;

	cout << endl << GREEN("connections:" + to_string(_state.size()) + " sockets:" + to_string(_server.count())) << endl << endl;
}

void Socks::close(int client)
{
	cout << "[close] " << client << endl;
	cout << (_server.close(client) ? GREEN("OK2") : RED("NG2")) << endl;
}

bool Socks::need_buf(int fd, string& message, bool cond)
{
	if (cond) {
		_buf[fd] = message;
		return true;
	}

	return false;
}
