#include "server.h"
#include "session.h"
#include "conf.h"


Server::Server(boost::asio::io_service& io_service, short port)
: acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
  resolver_(io_service), socket_(io_service),
  context_(boost::asio::ssl::context::sslv23) {

  context_.set_options(
      boost::asio::ssl::context::default_workarounds
      | boost::asio::ssl::context::no_sslv2
      | boost::asio::ssl::context::single_dh_use);
  //信任证书 http://www.360doc.com/content/14/0219/16/11029609_353843892.shtml
  //删除hsts http://classically.me/blogs/how-clear-hsts-settings-major-browsers
  context_.use_certificate_chain_file(Conf::ROOT_CERT);
  context_.use_private_key_file(Conf::ROOT_KEY, boost::asio::ssl::context::pem);

  do_accept();

}

void Server::do_accept() {
  acceptor_.async_accept(socket_,
    [this](const boost::system::error_code& ec)
    {
      if (!acceptor_.is_open()) return;

      if (!ec) std::make_shared<Session>(std::move(socket_), resolver_, context_)->start();

      do_accept();
    }
  );
}
