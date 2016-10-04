#pragma once
#include "Protocol/ITunnelLike.h"
#include "StateMachine/StateBuffer.h"
#include "Utils.h"

class IProxy
{
public:


};

class Proxy : public ITunnelLike, public IProxy
{
public:

    using AddrList = vector<Utils::SocketAddress>;

    Proxy(ClientServer& server, const AddrList& l) : ITunnelLike(server), _remote_address_list(l) {}

    //Protocol
    virtual void on_connect(int client) override;

    //ITunnelLike
    virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) override;

protected:

    unsigned int _rr_index = 0;

    AddrList _remote_address_list;

};
