#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using boost::asio::ip::tcp;
using socket_ptr = std::shared_ptr<tcp::socket>;

class Server
{
  public:
    Server(boost::asio::io_service& io_service, short port);

  private:
    void do_accept();


    tcp::acceptor acceptor_;
    tcp::resolver resolver_;
    tcp::socket socket_;
    boost::asio::ssl::context context_;
};

