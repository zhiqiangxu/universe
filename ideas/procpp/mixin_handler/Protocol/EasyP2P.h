#pragma once
#include "ReactHandler.h"
#include <string>
#include <map>
using namespace std;

// this is home made P2P

enum class EasyP2PState
{
    NICK,
    CMD
};

class IEasyP2P
{
public:
    virtual void on_list(int fd) = 0;
    virtual void on_info(int fd, string target) = 0;
    virtual void on_connect_target(int fd, string target) = 0;
};

class EasyP2P : public Protocol, public IEasyP2P, public Stateful<EasyP2PState>
{
public:
    using Protocol::Protocol;

    virtual void on_list(int fd) override;
    virtual void on_info(int fd, string target) override;
    virtual void on_connect_target(int fd, string target) override;

    virtual void on_connect(int fd) override;
    virtual void on_message(int fd, string message) override;
    virtual void on_close(int fd) override;


private:
    string _get_name_info(int fd);

    map<int, Utils::SocketAddress> _sock_addrs;

    map<int, string> _s2n;
    map<string, int> _n2s;
};
