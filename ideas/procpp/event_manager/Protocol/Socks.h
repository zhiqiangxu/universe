#pragma once
#include "Protocol.h"

class ISocks
{
public:
};

class Socks : public Protocol, public ISocks
{
public:
	using Protocol::Protocol;

	virtual handle(int fd) override;
};
