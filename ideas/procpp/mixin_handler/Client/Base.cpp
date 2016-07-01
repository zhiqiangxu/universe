#include "ReactHandler.h"

namespace C {

	Base::Base(const string& address, uint16_t port, bool auto_reconnect)
	{
		_auto_reconnect = auto_reconnect;

		_connect_addr_list.insert(Utils::to_addr(address, port));
	}

	bool Base::connect(const Utils::SocketAddress& connect_addr, bool re_connect)
	{
		if (re_connect) close(connect_addr);
		else if (_addr2socket.find(connect_addr) != _addr2socket.end()) return true;

		 auto socket = connect(reinterpret_cast<const struct sockaddr*>(&connect_addr), Utils::addr_size(connect_addr));

		if (socket > 0) {

			watch(socket, to_callbacks(get_protocol()));
			_addr2socket[connect_addr] = socket;
            _connect_addr_list.insert(connect_addr);

			return true;
		}

		return false;
	}

	bool Base::close(const Utils::SocketAddress& connect_addr)
	{
		if (_addr2socket.find(connect_addr) != _addr2socket.end()) {
			if (Client::close(_addr2socket[connect_addr], true)) {
				_addr2socket.erase(connect_addr);
				return true;
			} else {
				return false;
			}
		}

		return true;
	}


	Utils::SocketAddress Base::_get_next_address()
	{
		if (_connect_addr_list.size() == 0) L.error_exit("__connect_addr_list.size empty");

		return *std::next(_connect_addr_list.begin(),  _rr_index++ % _connect_addr_list.size());
	}
}
