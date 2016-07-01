#pragma once
#include "Protocol.h"
#include "Protocol/Soa.h"//SoaRequest,SoaResponse
#include <map>
using namespace std;

namespace P { namespace Client {

	using SoaCB = function<void(string&)>;

	class ISoa
	{
	public:

		enum class packet_type
		{
			JSON,
		};


	protected:
		virtual SoaResponse parse_response(StreamReader& s) = 0;
	};

	class Soa : public Base, public ISoa
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

		void add_callback(GUID& request_id, int fd, SoaCB callback);

		// required by delete keyword
		virtual ~Soa() {}

	protected:
		virtual SoaResponse parse_response(StreamReader& s) override;

		map<int, map<GUID, SoaCB>> _callbacks;
	};

}}
