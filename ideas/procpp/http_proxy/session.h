#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <memory>//std::move
#include <array>//std::array
#include <iostream>//std::cout
#include <string>//std::string
#include "http.h"
#include "cache.h"

using boost::asio::ip::tcp;
using boost::property_tree::ptree;

using socket_ptr = std::shared_ptr<tcp::socket>;
using ssl_socket = boost::asio::ssl::stream<tcp::socket&>;
using ssl_socket_ptr = std::shared_ptr<ssl_socket>;
using ssl_context_ptr = std::shared_ptr<boost::asio::ssl::context>;
using std::string;

class Session
  : public std::enable_shared_from_this<Session>
{
  public:
    Session(tcp::socket&& socket/*lvalue of rvalue reference*/, tcp::resolver& resolver, boost::asio::ssl::context& context);
    ~Session();

    void start();

  private:
    void do_read_http_request();
    void handle_read_http_request(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void do_write_407_unauthorized();
    void do_auth();
    void handle_auth();
    void do_after_auth();

    //处理非connect请求
    void do_direct_request();
    void handle_resolve_for_direct_request(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator);
    void handle_connect_for_direct_request(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator);
    void handle_write_for_direct_request(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void do_read_for_direct_request();
    void handle_read_for_direct_request(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void handle_response_for_direct_request(const boost::system::error_code& ec, std::size_t bytes_transferred);

    ///处理connect请求
    void do_connect_request();
    void handle_resolve_for_connect_request(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator);
    //后端connect成功后返回200
    void handle_connect_for_connect_request(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator);
    //connect成功后前端socket进行ssl握手
    void do_read_for_connect_request(const boost::system::error_code& ec);
    //前端socket ssl握手成功后，后端socket开始握手
    void handle_handshake_for_connect_request(const boost::system::error_code& ec);
    //前后端都握手成功后读取https包
    void do_read_after_handshake_for_connect_request();
    //尝试拼装https包
    void handle_read_after_handshake_for_connect_request(const boost::system::error_code& ec, std::size_t bytes_transferred);
    //https包拼装成功后进行转发
    void do_ssl_proxy_request_for_connect_request();
    //https包转发成功后读取响应
    void handle_ssl_proxy_request_for_connect_request(const boost::system::error_code& ec, std::size_t bytes_transferred);
    //https包转发后等待响应
    void do_ssl_read_for_connect_request();
    void handle_ssl_read_for_connect_request(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void handle_ssl_response_for_connect_request(const boost::system::error_code& ec, std::size_t bytes_transferred);

    void post_request();
    void post_response();
    void post_http_request(string url, string data);

    ssl_context_ptr context_for_domain(const string& domain);

    tcp::resolver& resolver_;
    boost::asio::ssl::context& context_;

    tcp::socket socket_;
    ssl_socket_ptr p_ssl_socket_;
    request_ptr p_request_;
    request_ptr p_connect_request_;
    std::array<char, 8192> buffer_;
    string packet_in_;

    socket_ptr p_socket2_;
    ssl_socket_ptr p_ssl_socket2_;
    response_ptr p_response2_;
    std::array<char, 8192> buffer2_;
    string packet2_in_;
    string packet2_out_;

    string filtered_response_;
    string auth_user_;



    GUID guid_;


    static const size_t MAX_REQUEST_PACKET_SIZE = 10*1024*1024;
    static const size_t MAX_RESPONSE_PACKET_SIZE = 10*1024*1024;
    static map<string, ssl_context_ptr> domain_context_;
    static Cache cached_credential_;
};

