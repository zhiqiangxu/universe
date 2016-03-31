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
				auto accept = Base64::encode(Utils::sha1(HttpToken::SEC_WEBSOCKET_KEY + GUID));

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
		}
		case WebSocketState::OPEN:
		{
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
				uint8_t first_byte;
				s.read_plain(first_byte);
				auto fin = first_byte >> 7;
				auto opcode = first_byte & 0x0F;

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

				string payload(payload, 0);
				s.read_size(payload_len, &payload[0]);

				if (mask) {
					for (int i = 0; i < payload_len; i++) {
						payload[i] = payload[i] ^ mask_value[ i % 4 ];
					}
				}

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
	}
}

void WebSocket::on_close(int client)
{
	erase_state_buffer(client);
}
