#include "Protocol/Client/Redis.h"
#include "ReactHandler.h"

using namespace P::Client;

void Redis::on_connect(int client)
{
	_fd = client;
}

void Redis::on_message(int client, string message)
{
	append_buf(client, message);

	RedisReply r;

	auto cb = _callbacks.front();
	_callbacks.pop();
	cb(r);
}

void Redis::on_close(int client)
{
	erase_buf(client);
}

void Redis::cmd_set(string key, string value, int* ex, int* px, NXXX* nxxx, RedisCB cb)
{
	string cmd_string = "";
	_scheduler.write(_fd, cmd_string.data(), cmd_string.length());

	_callbacks.push(cb);
}
