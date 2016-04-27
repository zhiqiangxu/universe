#pragma once
#include "Client/Base.h"
#include "Protocol/Client/Redis.h"

namespace C {
	class IRedis
	{
	};

	class Redis : public IRedis, public Base
	{
	public:
		//reused from Base
		using Base::Base;

		//override Base
		virtual P::Client::Base& get_protocol() override;

		//new stuff
		void set_shared_proto(P::Client::Redis* shared_proto);
		virtual ~Redis();

	protected:
		P::Client::Redis* _p_proto = nullptr;
		P::Client::Redis* _p_shared_proto = nullptr;//caller is responsible for delete shared proto
	};
}
