#pragma once
#include "Protocol/Client/Base.h"
#include <functional>
#include <queue>
#include <string>
#include <vector>

namespace P { namespace Client {
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
		enum class packet_type
		{
			SET,
			GET,
		};


	protected:
		virtual RedisReply parse_response(StreamReader& s) = 0;

		//bulk string
		static string resp_encode(string* p_bulk_string);
		static string resp_encode(char* p_bulk_string);
		static string resp_encode(const string& bulk_string);
		//integer
		static string resp_encode(uint64_t n);

		template<typename T1, typename... Tn>
		static string create_cmd(T1 arg1, Tn... args)
		{
			auto n = 1 + sizeof...(Tn);

			string cmd = "*" + to_string(n) + "\r\n" + resp_encode_elements(arg1, args...);

			return cmd;
		}

		template<typename T1, typename... Tn>
		static string resp_encode_elements(T1 arg1, Tn... args)
		{
			auto result = resp_encode(arg1);
			if (sizeof...(Tn) > 0) result += resp_encode_elements(args...);

			return result;
		}
		static string resp_encode_elements()
		{
			return "";
		}
	};

	class Redis : public Base, public IRedis
	{
	public:
		class ON_CONNECT {};
		class ON_MESSAGE {};
		class ON_CLOSE {};

		using Base::Base;

		// override for response parsing and callback
		virtual void on_message(int client, string message) override;
		virtual void on_close(int client) override;

		// implement for request
		template <packet_type type, typename... Args>
		static string packet(const GUID& request_id, const Args&... args);

		void add_callback(GUID& request_id, RedisCB callback);

		// required by delete keyword
		virtual ~Redis() {}


	protected:
		virtual RedisReply parse_response(StreamReader& s) override;


		queue<pair<GUID, RedisCB>> _callbacks;
	};

}}
