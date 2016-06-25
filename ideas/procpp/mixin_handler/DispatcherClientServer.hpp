template <DispatchMode m, typename Proto>
template <typename T>
typename enable_if<m != DispatchMode::Process, T>::type
    DispatcherClientServer<m, Proto>::listen(uint16_t port, int domain)
    {
        if (!_p_dispatcher) L.error_exit("You must call dispatch before listen");

        return ClientServer::listen(port, _p_dispatcher->to_callbacks(), domain);
    }

template <DispatchMode m, typename Proto>
template <typename T>
typename enable_if<m == DispatchMode::Process, T>::type
    DispatcherClientServer<m, Proto>::listen(uint16_t port, int domain)
    {

        if (!_p_proto) _p_proto = new Proto(*this);

        return ClientServer::listen(port, *_p_proto, domain);
    }


template <DispatchMode m, typename Proto>
DispatcherClientServer<m, Proto>::~DispatcherClientServer()
{
    if (_p_dispatcher) delete _p_dispatcher;

    if (_p_proto) delete _p_proto;
}
