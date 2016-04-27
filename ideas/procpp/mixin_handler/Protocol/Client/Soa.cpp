#include "ReactHandler.h"

namespace P { namespace Client {
	#include "Protocol/Client/Soa.hpp"

	void Soa::on_message(int client, string message)
	{
		append_buf(client, message);

		StreamReader s(message);

		size_t offset = 0;

		try {
			do {
				auto r = parse_response(s);
				offset = s.offset();

				if (_callbacks.find(r.uuid) == _callbacks.end()) {
					L.error_log("callback empty for soa response");
					continue;
				}

				auto cb = _callbacks[r.uuid];
				cb(r.json);
				_callbacks.erase(r.uuid);

				//notify wait
				_scheduler.fire<::Client::ON_PACKET_OK, GUID&>(r.uuid);

			} while (!s.end());

		} catch (ReaderException e) {
			//TODO error handle
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

	void Soa::on_close(int client)
	{
		erase_buf(client);
		_callbacks.clear();
	}

	
	SoaResponse Soa::parse_response(StreamReader& s)
	{
		SoaResponse r;

		s.read_plain(r.uuid);//这里假装不知道uuid是数组类型，对数组寻址其实是void操作

		size_t json_size;
		s.read_plain<true>(json_size);

		r.json.resize(json_size);
		s.read_size(json_size, &r.json[0]);

		return r;
	}
}}
