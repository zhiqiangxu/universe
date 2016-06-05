#pragma once
#include <type_traits>

class ClientServer;

enum class DispatchMode
{
	Base,
	ProcessSession,
	Remote,
	Proxy
};

/* Proto is not used for Remote/Proxy mode */
template <DispatchMode m, typename Proto>
class Dispatcher
{
};

template <typename Proto>
class Dispatcher<DispatchMode::Base, Proto>
{
public:
	template<typename... Args>
	Dispatcher( Args&&... args ) : _proto(std::forward<Args>(args)...) {};
	EventManager::EventCB to_callbacks() { return dynamic_cast<ClientServer&>(_proto.get_scheduler()).to_callbacks(_proto); }
protected:
	Proto _proto;
};

template <typename Proto>
class Dispatcher<DispatchMode::ProcessSession, Proto>
{
public:
	template<typename... Args>
	Dispatcher( Args&&... args ) : _worker(std::forward<Args>(args)...) {};
	EventManager::EventCB to_callbacks() { return _worker.get_server().to_callbacks(_worker); }

protected:
	ProcessSessionWorker<Proto> _worker;
};

template <typename Proto>
class Dispatcher<DispatchMode::Remote, Proto>
{
public:
	template<typename... Args>
	Dispatcher( Args&&... args ) : _worker(std::forward<Args>(args)...) {};
	EventManager::EventCB to_callbacks() { return _worker.get_server().to_callbacks(_worker); }

protected:
	RemoteWorker _worker;
};

template <typename Proto>
class Dispatcher<DispatchMode::Proxy, Proto>
{
public:
	template<typename... Args>
	Dispatcher( Args&&... args ) : _worker(std::forward<Args>(args)...) {};
	EventManager::EventCB to_callbacks() { return _worker.get_server().to_callbacks(_worker); }

protected:
	ProxyWorker _worker;
};


