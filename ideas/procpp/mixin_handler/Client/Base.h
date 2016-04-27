#pragma once
#include "Utils.h"
#include "Protocol/Client/Base.h"

// binds Client and Protocol into a working unit;
// only connects to a single address,
// if multiple addresses, it's caller's responsibility to maintain different instances


namespace C {

	class IBase
	{
	public:
		//同步连接
		virtual bool connect() = 0;
		virtual bool close() = 0;
		virtual P::Client::Base& get_protocol() = 0;
		bool get_auto_reconnect() { return _auto_reconnect; }
		void set_auto_reconnect(bool auto_reconnect) { _auto_reconnect = auto_reconnect; }


	protected:
		Utils::SocketAddress _connect_addr;
		bool _auto_reconnect;
	};

	class Base : public IBase, public Client//TODO maybe compose is more proper here
	{
	public:
		Base(const string& address, uint16_t port, bool auto_reconnect = true);

		//reused from Client
		using Client::connect;


		//override IBase
		virtual bool connect() override;
		virtual bool close() override;

		//new stuff
		template <typename Protocol, typename Protocol::packet_type type, typename cb_type, typename... Args>
		bool cmd(GUID& request_id, cb_type callback, const Args&... args)
		{
			if (_socket < 0) connect();

			auto packet = Protocol::template request_packet<Protocol, type>(request_id, args...);

			int times = 0;

		WRITE:
			//int errno = -1;
			auto bytes = write(_socket, packet.data(), packet.length()/*, &errno*/);

			if (bytes == -1 || (size_t)bytes != packet.length()) {
				//TODO check errno
				if (_auto_reconnect) {
					times += 1;
					if (times <= 1/*TODO 重连次数可配置*/) {
						connect();
						goto WRITE;
					}
				}

				return false;
			}

			static_cast<Protocol&>(get_protocol()).add_callback(request_id, callback);

			return true;
		}


	protected:
		int _socket;
	};
}
