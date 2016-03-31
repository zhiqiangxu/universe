#include "Protocol/WebSocket.h"

void WebSocket::on_connect(int client)
{
	set_state(client, WebSocketState::HANDSHAKE);
}

void WebSocket::on_message(int client, string message)
{
	append_buf(client, message);

	switch (get_state(client)) {
		case WebSocketState::HANDSHAKE:
		{
			HttpRequest r;

			break;
		}
		case WebSocketState::CONNECTED:
		{
			break;
		}
	}
}

void WebSocket::on_close(int client)
{
}
