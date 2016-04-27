#pragma once
#include "Client/Base.h"
#include "Protocol/Client/Soa.h"

namespace C {
	class ISoa
	{
	};

	class Soa : public ISoa, public Base
	{
	public:
		//reused from Base
		using Base::Base;

		//override Base
		P::Client::Base& get_protocol();

		//new stuff
		virtual ~Soa();

	protected:
		P::Client::Soa* _p_proto = nullptr;
	};

}

