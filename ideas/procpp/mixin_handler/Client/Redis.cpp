#include "ReactHandler.h"

namespace C {

    P::Client::Base& Redis::get_protocol()
    {
        if (_p_shared_proto) {
            return *_p_shared_proto;
        }

        if (!_p_proto) {
            _p_proto = new P::Client::Redis(*this);
        }

        return *_p_proto;
    }

    void Redis::set_shared_proto(P::Client::Redis* shared_proto)
    {
        _p_shared_proto = shared_proto;
    }

    Redis::~Redis()
    {
        if (_p_proto) {
            delete _p_proto;
            _p_proto = nullptr;
        }
    }

}
