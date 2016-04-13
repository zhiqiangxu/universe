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

			bool is_nil() { return type == ' '; }
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
			//commands
			virtual void set(string key, string value, RedisCB cb, int* p_ex, int* p_px, NXXX* p_nxxx) = 0;
			virtual void get(string key, RedisCB cb) = 0;

		protected:
			virtual RedisReply parse_response(StreamReader& s) = 0;

			//bulk string
			virtual string resp_encode(string* p_bulk_string) = 0;
			virtual string resp_encode(char* p_bulk_string) = 0;
			virtual string resp_encode(const string& bulk_string) = 0;
			//integer
			virtual string resp_encode(uint64_t n) = 0;

			template<typename T1, typename... Tn>
			string create_cmd(T1 arg1, Tn... args)
			{
				auto n = 1 + sizeof...(Tn);

				string cmd = "*" + to_string(n) + "\r\n" + resp_encode_elements(arg1, args...);

				return cmd;
			}

			template<typename T1, typename... Tn>
			string resp_encode_elements(T1 arg1, Tn... args)
			{
				auto result = resp_encode(arg1);
				if (sizeof...(Tn) > 0) result += resp_encode_elements(args...);

				return result;
			}
			string resp_encode_elements()
			{
				return "";
			}
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
			virtual void set(string key, string value, RedisCB cb, int* p_ex = nullptr, int* p_px = nullptr, NXXX* p_nxxx = nullptr) override;
			virtual void get(string key, RedisCB cb) override;

		protected:
			virtual RedisReply parse_response(StreamReader& s) override;

			virtual string resp_encode(string* p_bulk_string) override;
			virtual string resp_encode(char* p_bulk_string) override;
			virtual string resp_encode(const string& bulk_string) override;
			virtual string resp_encode(uint64_t n) override;

			queue<RedisCB> _callbacks;
			int _fd;
		};
	}
}
