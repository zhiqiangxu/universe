#pragma once
#include "Protocol.h"
#include "EventManager.h"
#include "StateMachine/StateBuffer.h"


class ClientServer;

class ITunnelLike : public StateBuffer<ConnectState>, public Protocol
{

public:

    ITunnelLike(ClientServer& server) : Protocol(server), _server(server) {}

    //子类实现
    virtual void on_connect(int client) = 0;
    //连上前缓存，连上后透传
    virtual void on_message(int client, string message);
    //本地断开时，关闭远程
    virtual void on_close(int client);

    //子类实现
    virtual void on_remote_connect(int remote, ConnectResult r, int client) = 0;
    //透传
    virtual void on_remote_message(int remote, string message, int client);
    //远程断开时，关闭本地
    virtual void on_remote_close(int remote, int client);

    void register_pair_info(int client, int remote);
    void erase_client_info(int client);

protected:

    ClientServer& _server;
    map<int, int> _c2r;
    map<int, int> _r2c;

};

