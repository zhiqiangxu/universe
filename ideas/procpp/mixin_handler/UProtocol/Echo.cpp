#include "UProtocol/Echo.h"
#include "ReactHandler.h"


namespace U {

    void Echo::on_message(int u_sock, string message, Utils::SocketAddress addr, socklen_t addrlen)
    {
        cout << "on_message #message = " + to_string(message.length()) << endl;
        _scheduler.sendto(u_sock, message.data(), message.length(), 0, reinterpret_cast<const struct sockaddr *>(&addr), Utils::addr_size(addr));
    }

}
