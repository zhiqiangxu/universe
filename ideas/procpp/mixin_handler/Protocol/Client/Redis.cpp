#include "Protocol/Client/Redis.h"
#include "ReactHandler.h"
#include <assert.h>

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

void Redis::cmd_set(string key, string value, RedisCB cb, int* p_ex, int* p_px, NXXX* p_nxxx)
{
	string cmd_string;
	if (px) {
		cmd_string = create_cmd();
	}

	_scheduler.write(_fd, cmd_string.data(), cmd_string.length());

	_callbacks.push(cb);
}

string Redis::resp_encode(string* p_bulk_string)
{
	if (p_bulk_string) {
		auto& bulk_string = *p_bulk_string;

		return resp_encode(bulk_string);
	} else return "$-1\r\n";//Null Bulk String
}

string Redis::resp_encode(const string& bulk_string)
{
	return "$" + to_string(bulk_string.length()) + "\r\n" + bulk_string + "\r\n";
}

string Redis::resp_encode(uint64_t n)
{
	return ":" + to_string(n) + "\r\n";
}
