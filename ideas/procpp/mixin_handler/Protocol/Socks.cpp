#include "Protocol/Socks.h"
#include "ClientServer.h"
#include <arpa/inet.h>//ntohs
#include <iostream>//cout
#include "Log/ErrorHandler.h"
#include "Utils.h"
using namespace std;



void Socks::on_connect(int client)
{
	cout << "[client] " << Utils::GREEN(to_string(client)) << endl;

	cout << endl << Utils::GREEN("connections:" + to_string(count_state()) + " sockets:" + to_string(_scheduler.count())) << endl << endl;

	set_state(client, SocksState::GREETING);
}

void Socks::on_message(int client, string message)
{
	append_buf(client, message);

	//cout << "message length: " << message.length() << endl;

	switch (get_state(client)) {
		case SocksState::GREETING:
		{
			if (need_buf(client, message, message.length() < 2)) return;

			if (message[0] != 0x05) {
				cout << Utils::RED("not 0x50") << endl;
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
				cout << Utils::RED("not no_auth") << endl;
				close(client);
				return;
			}

			string response("\x05\x0", 2);
			_scheduler.write(client, response.data(), response.length());

			set_state(client, SocksState::REQUEST);

			break;
		}
		case SocksState::REQUEST:
		{
			if (need_buf(client, message, message.length() < 5)) return;

			if (message[1] != 0x01) {
				cout << Utils::RED("not connect") << endl;
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
					cout << Utils::RED("invalid address type") << endl;
					close(client);
					return;
			}

			//cout << "required length: " << length << endl;

			if (need_buf(client, message, message.length() < length)) return;

			const uint16_t port = ntohs(*reinterpret_cast<const uint16_t*>(message.substr(length - 2, 2).c_str()));
			switch (message[3]) {
				case 0x01:
				{
					cout << Utils::RED("address type 0x01 not impl, terminate") << endl;
					close(client);
					return;
				}
				case 0x03:
				{
					auto address = message.substr(5, message[4]);
					cout << "connect remote:\t" << address << ":" << port << endl;

					auto remote_fd = dynamic_cast<ClientServer&>(_scheduler).connect(address, port, EventManager::EventCB{
						{
							EventType::CONNECT, EventManager::CB([client, this, port] (int remote_fd, ConnectResult r) {
								auto address = _url.find(client) != _url.end() ? _url[client] : "";
								if (r != ConnectResult::OK) {
									cout << Utils::RED("connect remote fail:" + address + " ") /*TODO << r */<< endl;
									close(client);
									return;
								}

								cout << Utils::GREEN("connect ok  " + address) << endl;
								cout << endl << Utils::GREEN("connections:" + to_string(count_state()) + " sockets:" + to_string(_scheduler.count())) << endl << endl;

								set_state(client, SocksState::CONNECTED);

								//cout << "suceeded: " << client << endl;
								string response("\x05\x00\x00\x03", 4);
								response.append(1, address.length());
								response.append(address);
								response.append(string(reinterpret_cast<const char*>(&port), sizeof(port)));
								_scheduler.write(client, response.data(), response.length());
								//cout << "response length: " << response.length() << endl;

							})
						},
						{
							EventType::READ, EventManager::CB([client, this] (int remote_fd, string message) {
								send_peer(client, message);
							})
						},
						{
							EventType::CLOSE, EventManager::CB([client, this] (int remote_fd) {
								cout << Utils::YELLOW("remote_fd should be " + to_string(remote_fd)) << endl;
								close(client);
							})
						}

					}, true);

					if (remote_fd == -1) {
						cout << Utils::RED("connect returns -1, close: " + to_string(client) + " " + address) << endl;
						close(client);
						return;
					}

					_c2r[client] = remote_fd;
					_r2c[remote_fd] = client;

					_url[client] = address;
					set_state(client, SocksState::CONNECTING);

					if (message.length() > length) {
						cout << Utils::RED("trigger next frame") << endl;
						on_message(client, message.substr(length));
					}

					break;
				}
				case 0x04:
				{
					cout << Utils::RED("address type 0x04 not impl, terminate") << endl;
					close(client);
					return;
				}
			}

			break;
		}
		case SocksState::CONNECTING:
		{
			cout << Utils::RED("should not send any more message while connecting") << endl;
			close(client);
			return;
		}
		case SocksState::CONNECTED:
		{
			//cout << "send_peer length: " << message.length()  << endl;
			send_peer(_c2r[client], message);
			break;
		}
	}

}

bool Socks::send_peer(int peer_fd, string& message)
{
	return _scheduler.write(peer_fd, message.data(), message.length());
}

void Socks::erase_state_buffer(int fd)
{
	erase_state(fd);
	erase_buf(fd);
}

void Socks::on_close(int client)
{
	auto address = _url.find(client) != _url.end() ? _url[client] : "";

	cout << "[on_close] " << Utils::GREEN(to_string(client) + " client " + address) << endl;
	if (!has_state(client)) {
		cout << "already closed" << endl;
		cout << endl << Utils::GREEN("connections:" + to_string(count_state()) + " sockets:" + to_string(_scheduler.count())) << endl << endl;
		return;//已关闭
	}

	erase_state_buffer(client);
	_url.erase(client);

	if (_c2r.find(client) == _c2r.end()) return;


	auto remote_fd = _c2r[client];

	cout << Utils::YELLOW("remote_fd is " + to_string(remote_fd)) << endl;

	_c2r.erase(client);
	_r2c.erase(remote_fd);

	cout << (_scheduler.close(remote_fd) ? Utils::GREEN("OK1") : Utils::RED("NG1")) << endl;;

	cout << endl << Utils::GREEN("connections:" + to_string(count_state()) + " sockets:" + to_string(_scheduler.count())) << endl << endl;
}

void Socks::close(int client)
{
	cout << "[close] " << client << endl;
	cout << (_scheduler.close(client) ? Utils::GREEN("OK2") : Utils::RED("NG2")) << endl;
}


