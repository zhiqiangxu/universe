#pragma once
#include "Protocol.h"

class IEcho
{
public:
};

class Echo : public Protocol, public IEcho
{
public:
    using Protocol::Protocol;

    virtual void on_message(int fd, string message) override;
    virtual void on_close(int fd) override;
};
