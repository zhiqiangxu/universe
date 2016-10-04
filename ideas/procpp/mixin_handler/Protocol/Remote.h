#pragma once
#include "Protocol/ITunnelLike.h"
#include "StateMachine/StateBuffer.h"
#include "Utils.h"

class IRemote
{
public:


};

class Remote : public ITunnelLike, public IRemote
{
public:

    using AddrList = vector<Utils::SocketAddress>;

    Remote(ClientServer& server, const AddrList& l, uint16_t port);

    //Protocol
    virtual void on_connect(int client) override;

    //ITunnelLike
    virtual void on_remote_connect(int remote_fd, ConnectResult r, int client) override;

protected:

    unsigned int _rr_index = 0;

    AddrList _remote_address_list;

};
