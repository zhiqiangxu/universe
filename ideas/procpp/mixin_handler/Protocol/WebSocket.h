#pragma once
#include "Protocol/Http.h"
#include "StateMachine/StateBuffer.h"
using namespace std;

enum class WebSocketState
{
	HANDSHAKE,
	OPEN
};

class WebSocketMessage
{
public:
	int client;
	uint8_t opcode;
	string payload;
};

class IWebSocket
{
public:
	static const uint8_t OPCODE_CONTINUE_FRAME = 0x0;
	static const uint8_t OPCODE_TEXT_FRAME = 0x1;
	static const uint8_t OPCODE_BINARY_FRAME = 0x2;
	static const uint8_t OPCODE_CONNECTION_CLOSE = 0x8;
	static const uint8_t OPCODE_PING = 0x9;
	static const uint8_t OPCODE_PONG = 0xA;

	static const uint16_t CLOSE_NORMAL= 1000;
	static const uint16_t CLOSE_PROTOCOL_ERROR = 1002;
	static const uint16_t CLOSE_DATA_ERROR = 1003;

	virtual bool send(int client, const string& message, uint8_t opcode, bool fin) = 0;
	virtual bool close(int client, uint16_t code, const char* reason) = 0;
	virtual bool pong(int client, const string& payload) = 0;
};

class WebSocket : public Http, public IWebSocket, public StateBuffer<WebSocketState>
{
public:

	//event tags
	class ON_MESSAGE {};

	virtual ~WebSocket() {}

	using Http::Http;

	virtual bool send(int client, const string& message, uint8_t opcode = OPCODE_TEXT_FRAME, bool fin = true) override;
	virtual bool close(int client, uint16_t code, const char* reason = "") override;
	virtual bool pong(int client, const string& payload) override;

	virtual void on_connect(int client) override;
	virtual void on_message(int client, string message) override;
	virtual void on_close(int client) override;

};
