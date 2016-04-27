#include "Protocol/SoaProxy.h"

void SoaProxy::on_connect(int client)
{
	auto soa_connection = connect(client);

	if (soa_connection < 0) _scheduler.close(client);

	_c2r[client] = soa_connection;
	_r2c[soa_connection] = client;
}

void SoaProxy::on_message(int client, string message)
{
	append_buf(client, message);


	StreamReader s(message);
	size_t offset = 0;

	try {
		do {
			auto r = parse_request(client, s);
			offset = s.offset();

			SoaResponse resp(r.uuid);
			_scheduler.fire<SoaProxy::ON_REQUEST, decltype(r)&, decltype(resp)&>(r, resp);

			auto output = resp.to_string();
			_scheduler.write(client, output.data(), output.length());

		} while ( !s.end() );
		
	} catch (ReaderException e) {
		switch (e) {
			case ReaderException::AG:
			{
				need_buf(client, offset > 0 ? message.substr(offset) : message, true);
				return;
			}
			case ReaderException::NG:
			{
				_scheduler.close(client);
				return;
			}
		}
	}
}

void SoaProxy::on_close(int client)
{
	erase_buf(client);
}

int SoaProxy::do_connect(int client, bool re_connect)
{
	if (!re_connect) {
		for (int i = 0; i < _remote_address_list.size(); i++) {

			auto addr_index = ++_rr_index % _remote_address_list.size();//so it actually connects from the second address

			auto addr = _remote_address_list[addr_index];

			if (_sockets.find(addr) == _sockets.end()) {
				auto remote = static_cast<ClientServer&>(_scheduler).connect(&addr, Utils::addr_size(addr));

				if (remote > 0) {

					_c2r[client] = remote;
					_r2c[remote] = client;
					_sockets[addr] = remote;

					watch_remote(remote);

				}

				return remote;
			}

			return _sockets[addr];

		}
	} else {
		auto old_remote = _c2r[client];
		_scheduler.close(old_remote);
		_r2c.erase(old_remote);

		// 重连
		auto addr_index = _rr_index % _remote_address_list.size();
		auto addr = _remote_address_list[addr_index];

		auto new_remote = static_cast<ClientServer&>(_scheduler).connect(&addr, Utils::addr_size(addr));

		if (new_remote > 0) {

			_c2r[client] = new_remote;
			_sockets[addr] = new_remote;

			watch_remote(remote);

		} else {
			_c2r.erase(client);
			_sockets.erase(addr);
		}


		return new_remote;

	}

}

bool SoaProxy::watch_remote(int remote)
{
	_scheduler.watch(remote, EventManager::EventCB{
		{
			EventType::READ, EventManager::CB([this] (int remote, string message) {
				on_remote_message(remote, message);
			}),
		},
		{
			EventType::CLOSE, EventManager::CB([this] (int remote) {
				on_remote_close(remote_fd);
			})
		}
	});

}

void SoaProxy::on_remote_message(int remote, string& message)
{
}

void SoaProxy::on_remote_close(int remote)
{
}

void SoaProxy::handle_connect_failure(const Addr& addr)
{
	//TODO various alarm ...
	L.error_log("handle_connect_failure");
}
