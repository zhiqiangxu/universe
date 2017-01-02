#pragma once
#include <boost/asio.hpp>
#include <functional>
#include <string>
#include <map>
#include "http.h"

using std::string;
using std::map;
using boost::asio::ip::tcp;
//单发后自动delete。。
class AsyncHttpClient {
  public:
    AsyncHttpClient(const string& url, const string& data, const string& content_type, boost::asio::io_service& io_service, tcp::resolver& resolver);
    using CB = std::function<void(bool, response_ptr)>;
    void post(CB cb);

  protected:

    void handle_resolve(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator);
    void handle_connect(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator);
    void do_read();
    void handle_response(const boost::system::error_code& ec, size_t bytes_transferred);
    void done(bool suc, response_ptr p_response = nullptr);

    boost::asio::io_service& io_service_;
    tcp::socket socket_;
    tcp::resolver& resolver_;
    string url_;
    string data_;
    string content_type_;
    map<string, string> url_parts_;
    string packet_;
    string packet_out_;
    std::array<char, 8192> buffer_;
    CB cb_;
};

