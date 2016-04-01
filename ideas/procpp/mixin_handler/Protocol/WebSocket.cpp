#include "ReactHandler.h"

void WebSocket::on_connect(int client)
{
	set_state(client, WebSocketState::HANDSHAKE);
}

static string GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

void WebSocket::on_message(int client, string message)
{
	append_buf(client, message);

	StreamReader s(message);

	switch (get_state(client)) {
		case WebSocketState::HANDSHAKE:
		{
			try {
				auto r = parse_request(client, s);
				auto it = r.headers.find(HttpToken::SEC_WEBSOCKET_KEY);
				s.fail_if(it == r.headers.end());
				auto accept = Base64::encode(Utils::sha1(it->second + GUID));

				HttpResponse resp;
				resp.status_code = 101;
				resp.reason_phrase = "Switching Protocols";
				resp.headers = {{"Upgrade", "websocket"}, {"Connection", "Upgrade"}, {"Sec-WebSocket-Accept", accept}, {"Sec-WebSocket-Version", "13"}};

				auto output = resp.to_string();

				_server.write(client, output.data(), output.length());

				set_state(client, WebSocketState::OPEN);

				s.fail_if(!s.end());

			} catch(ReaderException e) {
				switch (e) {
					case ReaderException::AG:
					{
						need_buf(client, message, true);
						return;
					}
					case ReaderException::NG:
					{
						_server.close(client);
						return;
					}
				}
			}
			break;
		}
		case WebSocketState::OPEN:
		{
			size_t offset = 0;;
			try {
/**
      0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+

**/
PARSE_MESSAGE:
				int fin;
				bool first_frame = true;
				WebSocketMessage m;
				m.client = client;
				do {
					uint8_t first_byte;
					s.read_plain(first_byte);
					fin = first_byte >> 7;
					auto opcode = first_byte & 0x0F;

					if (first_frame) {
						first_frame = false;
						m.opcode = opcode;
					}

					uint8_t second_byte;
					s.read_plain(second_byte);
					auto mask = second_byte >> 7;
					uint64_t payload_len = second_byte & 0x7F;

					if (payload_len == 0x7E) {
						uint16_t extended_payload_len;
						s.read_plain<true>(extended_payload_len);
						payload_len = extended_payload_len;
					} else if (payload_len == 0x7F) {
						uint64_t extended_payload_len;
						s.read_plain<true>(extended_payload_len);
						payload_len = extended_payload_len;
					}

					uint8_t mask_value[4];
					if (mask) {
						s.read_size(sizeof(mask_value), mask_value);
					}

					string payload(payload_len, 0);
					s.read_size(payload_len, &payload[0]);

					if (mask) {
						for (uint64_t i = 0; i < payload_len; i++) {
							payload[i] = payload[i] ^ mask_value[ i % 4 ];
						}
					}

					m.payload += payload;
				} while (!fin);

				offset = s.offset();

				_server.fire<WebSocket::ON_MESSAGE, decltype(m)&, WebSocket&>(m, *this);
				switch (m.opcode) {
					case OPCODE_TEXT_FRAME:
					case OPCODE_BINARY_FRAME:
					{
						break;
					}
					case OPCODE_CONNECTION_CLOSE:
					{
						close(client, m.payload.length() >= 2 ? *(uint16_t*)&m.payload[0] : CLOSE_NORMAL, "OPCODE_CONNECTION_CLOSE");
						return;
					}
					case OPCODE_PING:
					{
						pong(client, m.payload);
						break;
					}
					case OPCODE_PONG:
					{
						//ignore
						break;
					}
					default:
					{
						close(client, CLOSE_PROTOCOL_ERROR, "unkown opcode");
						return;
					}
				}

				if (!s.end()) goto PARSE_MESSAGE;
			} catch(ReaderException e) {
				switch (e) {
					case ReaderException::AG:
					{
						need_buf(client, offset > 0 ? message.substr(offset) : message, true);
						return;
					}
					case ReaderException::NG:
					{
						_server.close(client);
						return;
					}
				}

			}
			break;
		}
	}
}

void WebSocket::on_close(int client)
{
	erase_state_buffer(client);
}

bool WebSocket::close(int client, uint16_t code, const char* reason)
{
	string payload((char*)&code, 2);
	payload += reason;

	send(client, payload, OPCODE_CONNECTION_CLOSE);
	return _server.close(client);
}

bool WebSocket::pong(int client, const string& payload)
{
	return send(client, payload, OPCODE_PONG);
}

bool WebSocket::send(int client, const string& payload, uint8_t opcode, bool fin)
{
	string out;

	uint8_t first_byte = (fin << 7) + opcode;
	out += (char)first_byte;

	auto payload_len = payload.length();
	if (payload_len > 0xffff) {
		out += (char)0x7f;
		uint64_t len = payload_len;
		Utils::hton(len);
		out.insert(out.length(), (char*)&len, sizeof(len));
	} else if (payload_len > 0x7d) {
		out += (char)0x7e;
		uint16_t len = payload_len;
		Utils::hton(len);
		out.insert(out.length(), (char*)&len, sizeof(len));
	} else {
		uint8_t len = payload_len;
		out += (char)len;
	}
	out += payload;

	_server.write(client, out.data(), out.length());

	return true;
}
