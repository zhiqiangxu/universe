#include "ReactHandler.h"
#include <string.h>

void Soa::on_connect(int client)
{
}

void Soa::on_message(int client, string message)
{
	append_buf(client, message);


	StreamReader s(message);
	size_t offset = 0;

	try {
		do {
			auto r = parse_request(client, s);
			offset = s.offset();

			SoaResponse resp(r.uuid);
			_scheduler.fire<Soa::ON_REQUEST, decltype(r)&, decltype(resp)&>(r, resp);

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

void Soa::on_close(int client)
{
	erase_buf(client);
}


SoaRequest Soa::parse_request(int client, StreamReader& s)
{
	SoaRequest r;
	r.client = client;

	s.read_plain(r.uuid);

	size_t json_size;
	s.read_plain<true>(json_size);
	r.json.resize(json_size);
	s.read_size(json_size, &r.json[0]);

	return r;
}


string SoaResponse::to_string()
{
	return SoaRequest::to_string(uuid, json);
}

//called by soa client
string SoaRequest::to_string(const GUID& uuid, const string& json)
{
	string result;

	size_t json_size = json.length();

	result.resize(sizeof(uuid) + sizeof(size_t) + json_size);
	memcpy(&result[0], &uuid, sizeof(uuid));

	//convert to network byte order when transfer
	Utils::hton(json_size);

	memcpy(&result[sizeof(uuid)], &json_size, sizeof(json_size));
	memcpy(&result[sizeof(uuid) + sizeof(size_t)], &json[0], json.length());

	return result;
}
