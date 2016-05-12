#pragma once
#include "ClientServer.h"
#include "Protocol/Soa.h"
#include "Dispatch/Dispatcher.hpp"

class IDispatcherClientServer
{
public:
    virtual bool listen(uint16_t port, int domain) = 0;
};


template <DispatchMode m, typename Proto>
class DispatcherClientServer : public ClientServer, public IDispatcherClientServer
{
public:
	DispatcherClientServer() : _p_dispatcher(nullptr) {};
    virtual ~DispatcherClientServer();

    template <typename... Args>
    void dispatch( Args&&... args )
    {
        if (_p_dispatcher) L.error_exit("should not call dispatch twice");

        _p_dispatcher = new Dispatcher<m, Proto>(*this, std::forward<Args>(args)...);
    }

    virtual bool listen(uint16_t port, int domain = AF_INET) override;

private:
    Dispatcher<m, Proto>* _p_dispatcher;
};

#include "DispatcherClientServer.hpp"
