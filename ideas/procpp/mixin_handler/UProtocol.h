#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include "Utils.h"
using namespace std;

class Scheduler;

class IUProtocol
{
public:
    virtual ssize_t  get_buffer_size() = 0;
    virtual void on_message(int u_sock, string message, Utils::SocketAddress addr, socklen_t addrlen) = 0;

};

class UProtocol : public IUProtocol
{
public:

    virtual ssize_t get_buffer_size() override { return _buffer_size; };

    /*未实现on_message，是抽象类*/

    UProtocol(Scheduler& scheduler, uint64_t buffer_size = 1024*1024/*1M*/) : _scheduler(scheduler), _buffer_size(buffer_size) {}

protected:
    Scheduler& _scheduler;
    ssize_t _buffer_size;
};

