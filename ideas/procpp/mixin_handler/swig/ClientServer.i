class ClientServer
{
public:

    bool listen(uint16_t port, Protocol& proto, int domain = AF_INET);
    void event_loop();
    void set_worker_num(int worker_num);
    bool set_parent(string host, uint16_t port);
    void daemonize(const char* std_out = "/tmp/stdout.log", const char* std_err = "/tmp/stderr.log", const char* std_in = "/dev/null");
    ssize_t write_global(uint32_t session_id, const string& data);
};

%extend ClientServer {

    /* TODO investigate how to pass int pointer, and then remove this work around */
    long get_session_id(int client)
    {

        uint32_t session_id;
        auto ok = $self->get_session_id(client, &session_id);

        return ok ? session_id : -1;
    }

}
