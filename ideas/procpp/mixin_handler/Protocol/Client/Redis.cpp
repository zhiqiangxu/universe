#include "Protocol/Client/Redis.h"
#include "ReactHandler.h"
#include <assert.h>
#include <string.h>//strlen

using namespace P::Client;

void Redis::on_connect(int client)
{
	_fd = client;
}

void Redis::on_message(int client, string message)
{
	append_buf(client, message);

	StreamReader s(message);

	size_t offset = 0;

	try {
		auto r = parse_response(s);
		offset = s.offset();

		auto cb = _callbacks.front();
		_callbacks.pop();
		cb(r);

	} catch (ReaderException e) {
		//TODO error handle
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

RedisReply Redis::parse_response(StreamReader& s)
{
	RedisReply r;

	char next_byte;
	s.read_plain(next_byte);
	r.type = next_byte;

	switch(next_byte) {
		case '+':
		{
			s.read_until("\r", r.reply, true);
			s.read_plain(next_byte);
			s.fail_if(next_byte != '\r');
			break;
		}
		case '-':
		{
			s.read_until("\r", r.reply, true);
			s.read_plain(next_byte);
			s.fail_if(next_byte != '\r');
			break;
		}
		case ':':
		{
			break;
		}
		case '$':
		{
			break;
		}
		case '*':
		{
			break;
		}
	}

	return r;
}

void Redis::on_close(int client)
{
	erase_buf(client);
}

void Redis::set(string key, string value, RedisCB cb, int* p_ex, int* p_px, NXXX* p_nxxx)
{
	string element_expire;
	if (p_px) {
		element_expire = "PX " + to_string(*p_px);
	} else if (p_ex) {
		element_expire = "EX " + to_string(*p_ex);
	}

	string element_nxxx;
	if (p_nxxx) element_nxxx = Utils::enum_string(*p_nxxx);

	string cmd_string;
	if (element_expire.length()) {
		if (element_nxxx.length()) {
			cmd_string = create_cmd("SET", key, value, element_expire, element_nxxx);
		} else {
			cmd_string = create_cmd("SET", key, value, element_expire);
		}
	} else {
		if (element_nxxx.length()) {
			cmd_string = create_cmd("SET", key, value, element_nxxx);
		} else {
			cmd_string = create_cmd("SET", key, value);
		}
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

string resp_encode(char* p_bulk_string)
{
	auto len = strlen(p_bulk_string);
	return "$" + to_string(len) + "\r\n" + string(p_bulk_string, len) + "\r\n";
}

string Redis::resp_encode(const string& bulk_string)
{
	return "$" + to_string(bulk_string.length()) + "\r\n" + bulk_string + "\r\n";
}

string Redis::resp_encode(uint64_t n)
{
	return ":" + to_string(n) + "\r\n";
}
