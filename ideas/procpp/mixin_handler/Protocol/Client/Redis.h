#pragma once
#include "Protocol.h"
#include <functional>
#include <queue>
#include <string>
#include <vector>

namespace P
{
	namespace Client
	{
		using RedisReply = struct RedisReply {
			int type; /* REDIS_REPLY_* */
			
			string reply;
			vector<struct RedisReply> elements;
		};

		using RedisCB = function<void(RedisReply&)>;

		enum class NXXX {
			NX,
			XX
		};

		class IRedis
		{
		public:
			virtual void pipeline() = 0;
			virtual void exec() = 0;
			virtual void cmd_set(string key, string value, int* ex, int* px, NXXX* nxxx, RedisCB cb) = 0;
		};

		class Redis : public ::Protocol, public IRedis
		{
		public:
			class ON_CONNECT {};
			class ON_MESSAGE {};
			class ON_CLOSE {};

			using Protocol::Protocol;

			//只有connect成功才会调用，因为connect过程与具体协议无关
			virtual void on_connect(int client) override;
			virtual void on_message(int client, string message) override;
			virtual void on_close(int client) override;

			virtual void pipeline() override {};
			virtual void exec() override {};
			virtual void cmd_set(string key, string value, int* ex, int* px, NXXX* nxxx, RedisCB cb);

		protected:
			queue<RedisCB> _callbacks;
			int _fd;
		};
	}
}
