#include "ReactHandler.h"

int main()
{
	ClientServer s;

	WebSocket proto(s);
	s.listen(8082, proto);

	s.on<WebSocket::ON_MESSAGE>(Utils::to_function([](WebSocketMessage& m, WebSocket& proto) {
		proto.send(m.client, m.payload);
	}));

	s.start();
	return 0;
}
