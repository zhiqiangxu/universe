#pragma once
#include "Utils.h"
#include "Protocol/Client/Base.h"
#include <map>
#include <set>
using namespace std;

// binds Client and Protocol into a working unit;
// supports multiple addresses;

namespace C {

    using SocketAddress = Utils::SocketAddress;

	class IBase
	{
	public:
		//同步连接
		virtual bool connect(const SocketAddress& connect_addr, bool re_connect) = 0;
		virtual bool close(const SocketAddress& connect_addr) = 0;
		virtual P::Client::Base& get_protocol() = 0;
		bool get_auto_reconnect() { return _auto_reconnect; }
		void set_auto_reconnect(bool auto_reconnect) { _auto_reconnect = auto_reconnect; }


		IBase(bool auto_reconnect = true) : _auto_reconnect(auto_reconnect) {}

	protected:
		bool _auto_reconnect;
	};

	class Base : public IBase, public Client
	{
	public:
		Base(const string& address, uint16_t port, bool auto_reconnect = true);


		using IBase::IBase;
		//reused from Client
		using Client::connect;


		//override IBase
		virtual bool connect(const SocketAddress& connect_addr, bool re_connect = false) override;
		virtual bool close(const SocketAddress& connect_addr) override;

		//new stuff
		template <typename Protocol, typename Protocol::packet_type type, typename cb_type, typename... Args>
		bool cmd_addr(GUID& request_id, cb_type callback, const SocketAddress& address, const Args&... args)
		{

            if (cmd_addr<Protocol, type>(request_id, address, args...)) {

                auto socket = _addr2socket[address];
                static_cast<Protocol&>(get_protocol()).add_callback(request_id, socket, callback);
                return true;

            }

            return false;

		}

		template <typename Protocol, typename Protocol::packet_type type, typename... Args>
		bool cmd_addr(GUID& request_id, const SocketAddress& address, const Args&... args)
		{

			if ( !connect(address) ) return false;

			auto packet = Protocol::template request_packet<Protocol, type>(request_id, args...);

			int times = 0;

		WRITE:
			auto socket = _addr2socket[address];
			//int errno = -1;
			auto bytes = write(socket, packet.data(), packet.length()/*, &errno*/);

			if (bytes == -1 || (size_t)bytes != packet.length()) {
				//TODO check errno
				if (_auto_reconnect) {
					times += 1;
					if (times <= 1/*TODO retry次数可配置*/) {
						
						if ( !connect(address, true) ) return false;

						goto WRITE;
					}
				}

				return false;
			}

			return true;

		}


		template <typename Protocol, typename Protocol::packet_type type, typename cb_type, typename... Args>
		bool cmd(cb_type callback, GUID& request_id, const Args&... args)
        {
            auto address = _get_next_address();

            return this->cmd_addr<Protocol, type>(request_id, callback, address, args...);
        }

        // cmd without callback
        template <typename Protocol, typename Protocol::packet_type type, typename... Args>
        bool cmd(GUID& request_id, const Args&... args)
        {
            auto address = _get_next_address();

            return this->cmd_addr<Protocol, type>(request_id, address, args...);
        }

		template <typename Protocol, typename cb_type>
        void register_push_callback(cb_type callback)
        {
			static_cast<Protocol&>(get_protocol()).register_push_callback(callback);
        }



	protected:

		SocketAddress _get_next_address();

		set<SocketAddress> _connect_addr_list;
		map<SocketAddress, int> _addr2socket;
		int _rr_index;
	};
}
