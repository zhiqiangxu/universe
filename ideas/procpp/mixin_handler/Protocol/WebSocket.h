#pragma once
#include "Protocol/Http.h"
using namespace std;

class WebSocketState
{
	HANDSHAKE,
	CONNECTED
};

class IWebSocket
{
public:
};

class WebSocket : public Http, public IWebSocket
{
public:

	//event tags
	class ON_REQUEST {};


	using Protocol::Protocol;

	virtual void on_connect(int client) override;
	virtual void on_message(int client, string message) override;
	virtual void on_close(int client) override;

};
