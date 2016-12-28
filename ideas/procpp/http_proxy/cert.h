#pragma once
#include <memory>//std::shared_ptr
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>

using context_ptr = std::shared_ptr<boost::asio::ssl::context>;
using string = std::string;


class Cert {
  public:
    static string cert_for_domain(const string& domain, const string& csr_key, const string& ca_cert, const string& ca_key, bool& ok);

};
