#pragma once
#include <type_traits>

enum class DispatchMode
{
	Base,
	Process,
	Remote,
	Proxy
};

template <DispatchMode m, typename Proto>
class DispatcherMembers
{
};

template <Proto>
class DispatcherMembers<Base>
{
protected:
	Proto _proto;
};

template <Proto>
class DispatcherMembers<Process>
{
protected:
	ProcessWorker<Proto> _worker;
};

template <Proto>
class DispatcherMembers<Remote>
{
protected:
	RemoteWorker<Proto> _worker;
};

template <Proto>
class DispatcherMembers<Proxy>
{
protected:
	ProxyWorker<Proto> _worker;
};

template <DispatchMode m, typename Proto>
class Dispatcher : public DispatcherMembers<m, Proto>
{
public:
	EventManager::EventCB callbacks();

};


