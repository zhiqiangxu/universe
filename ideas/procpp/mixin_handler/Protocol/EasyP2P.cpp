#include "Protocol/EasyP2P.h"
#include <regex>

void EasyP2P::on_connect(int sock)
{
	Utils::SocketAddress addr;
	if (!Utils::get_peer_name(sock, addr)) {
		_scheduler.close(sock);
		return;
	}

	_sock_addrs[sock] = addr;

	string hello = "Please type your nick name";
	_scheduler.write_line(sock, hello);

	set_state(sock, EasyP2PState::NICK);
}

void EasyP2P::on_message(int sock, string message)
{
	append_buf(sock, message);

	switch (get_state(sock)) {
		case EasyP2PState::NICK:
		{
			string result;
			if ( !read_until(message, "\n", result, 0) ) {
				need_buf(sock, message, true);
				return;
			}

			regex r(R"(^(\w+)\s+)");
			smatch m;
			if (!regex_search(result, m, r)) {
				string message = "Invalid nickname!";
				_scheduler.write_line(sock, message);
				return;
			}

			auto nick = m[1];
			if (_n2s.find(nick) != _n2s.end()) {
				string message = "ALready exist!";
				_scheduler.write_line(sock, message);
				return;
			}

			_n2s[nick] = sock;
			_s2n[sock] = nick;
			_scheduler.write_line(sock, "Nickname OK!");
			_scheduler.write_line(sock, "You can type cmd now!");
			set_state(sock, EasyP2PState::CMD);

			break;
		}
		case EasyP2PState::CMD:
		{
			string result;
			regex r(R"(^\w+\s+(\w+))");

			while (true) {
				if ( !read_until(message, "\n", result, 0) ) {
					need_buf(sock, message, true);
					return;
				}

				if ( result.find("list") == 0 ) {
					on_list(sock);
				}
				else if ( result.find("info ") == 0 ) {
					{
						smatch m;
						if (!regex_search(result, m, r)) {
							L.debug_log(result + " not match");
							_scheduler.close(sock);
							return;
						}

						on_info(sock, m[1]);
					}
				}
				else if ( result.find("connect ") == 0 ) {
					{
						smatch m;
						if (!regex_search(result, m, r)) {
							L.debug_log(result + " not match");
							_scheduler.close(sock);
							return;
						}

						on_connect_target(sock, m[1]);
					}

				}

				if ( !message.length() ) return;
			}
			break;
		}
	}

}

void EasyP2P::on_list(int sock)
{
	L.debug_log("on_list");

	for (const auto& r : _n2s) {
		_scheduler.write_line(sock, r.first);
	}

}

void EasyP2P::on_info(int sock, string target)
{
	L.debug_log("on_info " + target);

	if (_n2s.find(target) == _n2s.end()) {
		_scheduler.write_line(sock, target + " not exists!");
		return;
	}

	auto target_sock = _n2s[target];
	auto name_info = _get_name_info(target_sock);

	_scheduler.write_line(sock, name_info);
	L.debug_log("on_info " + target);
}

void EasyP2P::on_connect_target(int sock, string target)
{
	L.debug_log("on_connect " + target);

	if (_n2s.find(target) == _n2s.end()) {
		_scheduler.write_line(sock, target + " not exists!");
		return;
	}

	auto target_sock = _n2s[target];
	auto name_info = _get_name_info(sock);
	_scheduler.write_line(target_sock, "/connect " + name_info);

	_scheduler.write_line(sock, "ok");

}

void EasyP2P::on_close(int sock)
{
	_sock_addrs.erase(sock);
	auto nick = _s2n[sock];
	_s2n.erase(sock);
	_n2s.erase(nick);
}

string EasyP2P::_get_name_info(int fd)
{
	auto addr = _sock_addrs[fd];
	return Utils::get_name_info(addr);
}
