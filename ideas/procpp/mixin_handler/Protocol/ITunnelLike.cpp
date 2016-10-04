#include "ReactHandler.h"

void ITunnelLike::register_pair_info(int client, int remote)
{
    _c2r[client] = remote;
    _r2c[remote] = client;
}

void ITunnelLike::erase_client_info(int client)
{
    erase_state_buffer(client);

    if (_c2r.find(client) != _c2r.end()) {
        auto remote = _c2r[client];
        _r2c.erase(remote);
        _c2r.erase(client);
    }

}

void ITunnelLike::on_message(int client, string message)
{

    //**this** is MUST:  http://stackoverflow.com/a/19129605
    append_buf(client, message);

    //DEBUG(this->has_state(client), "on_message fired when client already removed");
    //DEBUG(_c2r[client] == remote, "remote of client[" + to_string(client) + "] should be [" + to_string(remote) + "], but got [" + to_string(_c2r[client]) + "]");
    //DEBUG(_r2c[remote] == client, "client of remote[" + to_string(remote) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote]) + "]");

    //L.debug_log("get_state = " + Utils::enum_string(this->get_state(client)));
    if (need_buf(client, message, this->get_state(client) != ConnectState::CONNECTED)) {
        //L.debug_log("connecting...buffed");
        return;
    }

    //L.debug_log("write to remote " + to_string(remote));
    _server.write(_c2r[client], message.data(), message.length());

}

void ITunnelLike::on_close(int client)
{

    if (_c2r.find(client) == _c2r.end()) return;

    auto remote = _c2r[client];
    //L.debug_log("on_close client = " + to_string(client) + " remote = " + to_string(remote));

    erase_client_info(client);

    _server.close(remote);

}

void ITunnelLike::on_remote_message(int remote, string message, int client)
{

    //L.debug_log("on_remote_message: client = " + to_string(client) + " remote = " + to_string(remote));

    //DEBUG(this->has_state(client), "on_remote_message fired when client already removed");
    //DEBUG(_c2r[client] == remote, "remote of client[" + to_string(client) + "] should be [" + to_string(remote) + "], but got [" + to_string(_c2r[client]) + "]");
    //DEBUG(_r2c[remote] == client, "client of remote[" + to_string(remote) + "] should be [" + to_string(client) + "], but got [" + to_string(_r2c[remote]) + "]");

    _server.write(client, message.data(), message.length());

}

void ITunnelLike::on_remote_close(int remote, int client)
{
    //L.debug_log("on_remote_close: enter");

    if (_c2r.find(client) == _c2r.end()) return;

    //L.debug_log("on_remote_close client = " + to_string(client) + " remote = " + to_string(remote));

    erase_client_info(client);

    _server.close(client);

}


