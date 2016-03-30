#pragma once
#include "ReactHandler.h"

namespace Protocol::Client
{

	class IEasyP2P
	{
	public:

	};

	class EasyP2P : public IEasyP2P, public ::Protocol,
	{
	public:

		virtual void on_message(int fd, string message) override;
	};

}
