#include "ReactHandler.h"

Remote::Remote(ClientServer& server, const AddrList& l, uint16_t port) : ITunnelLike(server), _remote_address_list(l)
{
    _server.listen_for_child(port);
}

void Remote::on_connect(int client)
{

    this->set_state(client, ConnectState::B4CONNECT);

    auto addr_index = _rr_index++ % _remote_address_list.size();

    auto addr = _remote_address_list[addr_index];

    L.debug_log(Utils::get_name_info(addr));

    auto remote = _server.connect(reinterpret_cast<const struct sockaddr*>(&addr), Utils::addr_size(addr), EventManager::EventCB{
        {
            EventType::CONNECT, EventManager::CB([this, client] (int remote, ConnectResult r) {
                on_remote_connect(remote, r, client);
            }),
        },
        {
            EventType::READ, EventManager::CB([this, client] (int remote, string message) {
                on_remote_message(remote, message, client);
            })
        },
        {
            EventType::CLOSE, EventManager::CB([this, client] (int remote) {
                on_remote_close(remote, client);
            })
        },

    }, true);

    if (remote == -1) return;//on_remote_connect会关闭client

    // AF_UNIX的情况，一般情况下，此时已经CONNECTED
    if (this->get_state(client) == ConnectState::B4CONNECT) this->set_state(client, ConnectState::CONNECT);

    register_pair_info(client, remote);

}

//TODO 目测没有关闭remote的地方..
void Remote::on_remote_connect(int remote, ConnectResult r, int client)
{

    L.debug_log("on_remote_connect: "  + Utils::enum_string(r) + " client = " + to_string(client) + " remote = " + to_string(remote));

    if (r == ConnectResult::OK) {
        _server.add_session_task(client, [remote, this] (int client) {

            _server.send_session_id(remote, client);

            //L.debug_log("client " + to_string(client) + " set_stat CONNECTED");
            this->set_state(client, ConnectState::CONNECTED);


            //trigger on_message once in case buffered
            if (has_buf(client)) on_message(client, get_buf(client));

        });

    } else {

        erase_client_info(client);

        _server.close(client);
    }

}


