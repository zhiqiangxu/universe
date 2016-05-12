
template <DispatchMode m, typename Proto>
bool DispatcherClientServer<m, Proto>::listen(uint16_t port, int domain)
{
    if (!_p_dispatcher) L.error_exit("You must call dispatch before listen");

    return ClientServer::listen(port, _p_dispatcher->to_callbacks(), domain);
}

template <DispatchMode m, typename Proto>
DispatcherClientServer<m, Proto>::~DispatcherClientServer()
{
    if (_p_dispatcher) delete _p_dispatcher;
}
