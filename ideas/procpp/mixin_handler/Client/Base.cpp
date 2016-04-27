#include "ReactHandler.h"

namespace C {

	Base::Base(const string& address, uint16_t port, bool auto_reconnect)
	{
		_connect_addr = Utils::to_addr(address, port);
		_auto_reconnect = auto_reconnect;
		_socket = -1;
	}

	bool Base::connect()
	{
		 _socket = connect(reinterpret_cast<const struct sockaddr*>(&_connect_addr), Utils::addr_size(_connect_addr));

		if (_socket > 0) {

			watch(_socket, to_callbacks(get_protocol()));

			return true;
		}

		return false;
	}

	bool Base::close()
	{
		if (_socket > 0) return Client::close(_socket);

		return true;
	}

}
