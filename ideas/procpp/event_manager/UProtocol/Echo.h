#pragma once
#include "UProtocol.h"

namespace U
{
	class IEcho
	{
	};

	class Echo : public IEcho, public UProtocol
	{
	public:
		using UProtocol::UProtocol;

		virtual void on_message(int u_sock, string message, ::Utils::SocketAddress addr, socklen_t addrlen) override;
	};
}
