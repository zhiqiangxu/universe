#pragma once
#include "Protocol.h"

/**
this syntax is only available in gcc-6 and later,
which at the time of writing is not released yet...
namespace Server::Protocol
***/
class Global : public Protocol
{
public:

    using Protocol::Protocol;
    virtual ~Global() {}

    virtual void on_connect(int child) override;
    virtual void on_message(int child, string message) override;
    virtual void on_close(int child) override;
};
