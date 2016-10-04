#pragma once

#include "Server.h"
#include "Client.h"
#include "Server/Protocol/Global.h"


class IClientServer
{
public:
    using SessionTask = function<void(int)>;

    virtual bool get_session_id(int client, uint32_t* session_id) = 0;
    //将session_id发送给worker
    virtual bool send_session_id(int worker_fd, int client) = 0;
    virtual bool recv_session_id(int worker_client, string& message) = 0;
    virtual ssize_t write_global(uint32_t session_id, const void *buf, size_t count) = 0;
    virtual ssize_t write_global(uint32_t session_id, const string& data) = 0;

    virtual bool listen_for_child(uint16_t port) = 0;
    virtual bool listen_for_child(const string sun_path) = 0;
    virtual bool set_parent(string sun_path) = 0;
    virtual bool set_parent(string host, uint16_t port) = 0;
    virtual bool connect_parent() = 0;

    //need to call recv_session_id first for worker
    virtual EventManager::CB initial_message_wrapper(EventManager::CB::R r) = 0;
    virtual bool add_session_task(int client, SessionTask task) = 0;

    //默认Base模式，如设定worker_num，切换为多进程模式
    virtual void set_worker_num(int worker_num) = 0;

};

class ClientServer : public Client, public Server, public IClientServer, public Bufferable
{
public:
    static const int NUMBER_CORES = -1;

    virtual ~ClientServer();

    virtual int accept(int socketfd, struct sockaddr *addr, socklen_t *addrlen) override;
    virtual bool unwatch(int fd, bool no_callback = false) override;

    virtual bool get_session_id(int client, uint32_t* session_id) override;
    virtual bool send_session_id(int worker_fd, int client) override;//from master
    virtual bool recv_session_id(int worker_client, string& message) override;//from worker
    virtual ssize_t write_global(uint32_t session_id, const void *buf, size_t count) override;
    virtual ssize_t write_global(uint32_t session_id, const string& data) override;

    virtual bool listen_for_child(uint16_t port) override { return false; };
    virtual bool listen_for_child(const string sun_path) override;
    virtual bool set_parent(string sun_path) override;
    virtual bool set_parent(string host, uint16_t port) override;
    virtual bool connect_parent() override;

    virtual EventManager::CB initial_message_wrapper(EventManager::CB::R r) override;
    virtual bool add_session_task(int client, SessionTask task) override;
    virtual void set_worker_num(int worker_num) override { _worker_num = worker_num; };

    using Server::to_callbacks;
    using Client::to_callbacks;
    virtual EventManager::EventCB to_callbacks(Protocol& proto) override;
    //工作模式的切换在这里实现
    virtual void event_loop() override;

    bool _is_child = false;

    //session_id
    uint32_t _session_id = 0;
    map<int, uint32_t> _c2s;
    map<uint32_t, int> _s2c;

    //tasks that MUST be after recv_session_id succeeds
    //empty if master
    map< int, vector<SessionTask> > _session_tasks;

    //used for child/parent communication
    enum class AddressType
    {
        SUN,
        INET
    };
    AddressType _parent_addr_type;
    string _parent_sun;
    string _parent_host;
    uint16_t _parent_port;

    int _parent_sock = -1;

    int _worker_num = 0;

    Global* _global_proto_p = nullptr;

};
