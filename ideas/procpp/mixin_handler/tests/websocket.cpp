#include "ReactHandler.h"

int main()
{
	ClientServer s;

	WebSocket ws(s);
	s.listen(8082, ws);

	s.on<WebSocket::ON_MESSAGE>(Utils::to_function([](WebSocketMessage& m, WebSocket& ws) {
        if (m.opcode == ws.OPCODE_TEXT_FRAME) ws.send(m.client, m.payload);
	}));

	s.start();
	return 0;
}
