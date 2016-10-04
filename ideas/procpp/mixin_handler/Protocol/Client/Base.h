#pragma once
#include "Protocol.h"
#include <stdlib.h>//exit
#include <string>
using namespace std;

class Client;

namespace P { namespace Client {


    class Base : public ::Protocol
    {
    public:

        Base(::Client& client) : Protocol::Protocol(client) {}

        using Protocol::Protocol;

        template <typename Protocol, typename Protocol::packet_type type, typename... Args>
        static string request_packet(GUID& request_id, const Args&... args) {
            //request_id is generated in Base, then passed to Child
            request_id.generate();
            return Protocol::template packet<type, Args...>(request_id, args...);
        }


    protected:
    };

}}
