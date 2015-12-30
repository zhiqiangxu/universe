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

	virtual void handle(int fd) override;
};
