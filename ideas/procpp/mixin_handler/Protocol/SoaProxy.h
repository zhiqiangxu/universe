#pragma once
#include "Protocol/Soa.h"

/* 设计思路
 *
 * 类似Proxy
 * 不过与每个remote保持单一链接remote_socket
 * 所以实现上，记录每个client对应的remote_socket
 * 当client来请求时，转发到对应的remote_socket，并维持client和消息uuid的映射关系
 * 当remote来响应时，根据uuid转发到对应的client
 * 当client断开时，清空对应的映射关系
 * 当remote断开时，清空**之前**client的请求，并以某种方式【json.length() = 0】告诉client那些请求废了，别等了
 * 当client消息不合法，主动断开
 * 当remote响应不合法，主动断开重连
 *
 * 为防止accept太多的pending请求，以同步方式建立与remote的链接(设定较短的connect timeout)
 * TODO 优化，多进程实现每个进程维持单一remote_socket
 **/
class ISoaProxy
{
protected:
    // 返回一个已建立的soa connection
    virtual int connect(int client, bool re_connect = false) = 0;
    // 链接并关联两个socket
    virtual int do_connect(int client, bool re_connect = false) = 0;
    virtual bool watch_remote(int remote) = 0;
    virtual void on_remote_message(int remote, string& message) = 0;
    virtual void on_remote_close(int remote) = 0;
    virtual void handle_connect_failure(const Addr& addr) = 0;

};

//similar to Proxy
class SoaProxy : public Protocol, public Soa
{
public:
    using Addr = Utils::SocketAddress;
    using AddrList = vector<Addr>;
    SoaProxy(ClientServer& server, AddrList l, int connect_timeout = -1) : Protocol(server), _remote_address_list(l), _connect_timeout(connect_timeout) {}

    //override from Protocol
    virtual void on_connect(int client) override;
    virtual void on_message(int client, string message) override;
    virtual void on_close(int client) override;

protected:

    // 返回一个已建立的soa connection
    // 若指定非负index，说明该链接需要重连
    virtual int connect(int client, bool re_connect = false) override;
    // 链接并关联两个socket
    virtual int do_connect(int client, bool re_connect = false) override;
    virtual bool watch_remote(int remote) override;
    virtual void on_remote_message(int remote, string& message) override;
    virtual void on_remote_close(int remote) = 0;
    virtual void handle_connect_failure(const Addr& addr) override;

    map<int, int> _c2r;
    map<int, int> _r2c;

    unsigned int _rr_index = 0;
    AddrList _remote_address_list;
    int _connect_timeout;
    map<Addr, int> _fail_times;
    map<Addr, int> _sockets;
};
