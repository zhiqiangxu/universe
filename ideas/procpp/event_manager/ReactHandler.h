#pragma once
#define DEBUG(exp, message) {\
	if (!(exp)) L.error_exit(message); \
}


#define L ErrorHandler::get_instance()

#include "EventManager.h"
#include "EventHook.hpp"
#include "Server.h"
#include "ClientServer.h"
#include "HttpClientServer.h"
#include "Client.h"
#include "Protocol.h"
#include "Log/ErrorHandler.h"
#include "Utils.h"
#include "StateMachine/Stateful.h"
#include "StateMachine/Bufferable.h"
#include "StateMachine/StateBuffer.h"
#include "Dispatch/IBaseWorker.h"
#include "Dispatch/ProcessWorker.h"
#include "Dispatch/ProxyWorker.h"
#include "Dispatch/RemoteWorker.h"
#include "UProtocol/Echo.h"
#include "Protocol/Echo.h"
#include "Protocol/Http.h"




