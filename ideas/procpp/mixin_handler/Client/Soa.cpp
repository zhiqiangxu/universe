#include "ReactHandler.h"

namespace C {
	P::Client::Base& Soa::get_protocol()
	{
		if (!_p_proto) {
			_p_proto = new P::Client::Soa(*this);
		}

		return *_p_proto;
	}

	Soa::~Soa()
	{
		if (_p_proto) {
			delete _p_proto;
			_p_proto = nullptr;
		}
	}

}
