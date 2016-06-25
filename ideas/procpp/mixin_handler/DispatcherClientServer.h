#pragma once
#include "ClientServer.h"
#include "Protocol/Soa.h"
#include "Dispatch/Dispatcher.hpp"

class IDispatcherClientServer
{
public:
};


template <DispatchMode m, typename Proto>
class DispatcherClientServer : public ClientServer, public IDispatcherClientServer
{
public:
	DispatcherClientServer() : _p_dispatcher(nullptr), _p_proto(nullptr) {};
    virtual ~DispatcherClientServer();

    template <typename... Args>
    void dispatch( Args&&... args )
    {
        if (_p_dispatcher) L.error_exit("should not call dispatch twice");

        _p_dispatcher = new Dispatcher<m, Proto>(*this, std::forward<Args>(args)...);
    }

    // enable_if 2要素
    // 1. template method
    // 2. depends on template method typename
    template <typename T = bool>
    typename enable_if<m != DispatchMode::Process, T>::type
    listen(uint16_t port, int domain = AF_INET);

    template <typename T = bool>
    typename enable_if<m == DispatchMode::Process, T>::type
    listen(uint16_t port, int domain = AF_INET);

private:
    Dispatcher<m, Proto>* _p_dispatcher;

    Proto* _p_proto;//used by Process mode
};

#include "DispatcherClientServer.hpp"
