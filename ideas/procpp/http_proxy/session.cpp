#include <sstream>//ostringstream
#include <strings.h>//strcasecmp,strlen
#include "session.h"
#include "utils.h"
#include "async_http_client.h"
#include "cert.h"
#include "conf.h"

using boost::property_tree::write_json;
using boost::property_tree::ptree_error;
using std::stringstream;
using std::cout;
using std::endl;

#define PROLOGUE cout << "#################Session #" << (intptr_t)this << ": " << __func__ << endl;auto self(shared_from_this());

extern string PACKET_POST_URL;

Cache Session::cached_credential_;
map<string, ssl_context_ptr> Session::domain_context_;


Session::Session(tcp::socket&& socket/*lvalue of rvalue reference*/, tcp::resolver& resolver, boost::asio::ssl::context& context)
: resolver_(resolver), context_(context), socket_(std::move(socket)) {
}

Session::~Session() {
  cout << "#################Session #" << (intptr_t)this << " complete###################" << endl;
}

void Session::start() {
  cout << "#################Session #" << (intptr_t)this << " start###################" << endl;
  do_read_http_request();
}

void Session::do_read_http_request() {
  PROLOGUE;

  socket_.async_read_some(
    boost::asio::buffer(buffer_),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
      handle_read_http_request(ec, bytes_transferred);
    }
  );
}

void Session::handle_read_http_request(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  PROLOGUE;

  if (!ec) {
    packet_in_.append(buffer_.data(), bytes_transferred);
    if (packet_in_.length() > Session::MAX_REQUEST_PACKET_SIZE) return;

    try {
      size_t parsed_length;
      p_request_ = HttpRequest::parse_request(packet_in_, &parsed_length);
      //cout << packet_in_ << endl;
      //for (auto it:p_request_->headers) cout << it.first << " : " << it.second << endl;
      if (p_request_->headers.find("Proxy-Authorization") == p_request_->headers.end()) {
        do_write_407_unauthorized();
      } else do_auth();

      //pipelined request
      packet_in_= packet_in_.substr(parsed_length);
    } catch (ReaderException e) {
      switch (e) {
        case ReaderException::AG:
        {
          do_read_http_request();
          break;
        }
        case ReaderException::NG:
        {
          break;
        }
      }
    }
  }
}

void Session::do_write_407_unauthorized() {
  PROLOGUE;

  cout << "Unauthorized:" << endl << p_request_->forward_proxy_packet(true) << endl;

  filtered_response_ = p_request_->http_version + " 407 Proxy Authentication Required\r\n"
                  "Proxy-Authenticate: Basic realm=\"xdebug\"\r\n"
                  "Content-Type: text/html\r\n"
                  "Content-Length: " + std::to_string(strlen("Proxy Authentication Required")) + "\r\n"
                  "\r\n"
                  "Proxy Authentication Required";
  boost::asio::async_write(socket_, boost::asio::buffer(filtered_response_.data(), filtered_response_.length()),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      if (!ec) do_read_http_request();
    }
  );

}

void Session::do_auth() {
  PROLOGUE;

  auto auth = p_request_->headers["Proxy-Authorization"];
  auto pos = auth.find("Basic ");
  if (pos == string::npos) return;

  auto base64_value = auth.substr(pos + strlen("Basic "));
  auto value = Utils::base64_decode(base64_value);
  pos = value.find(":");
  if (pos == string::npos) return;

  auto user = value.substr(0, pos);
  auth_user_ = user;
  auto pass = pos == (value.length() - 1) ? "" : value.substr(pos + 1);
  string cached_pass;
  if (cached_credential_.get(user, cached_pass)) {
    if (cached_pass != pass) return;

    do_after_auth();
    return;
  }

  auto p_client = new AsyncHttpClient(
    "http://api.ffan.com/rtx_verify",
    "name=" + Utils::url_encode(user) + "&password=" + Utils::url_encode(pass),
    "application/x-www-form-urlencoded",
    socket_.get_io_service(),
    resolver_
  );

  p_client->post([self, this, user, pass](bool ok, response_ptr p_response) {
    stringstream ss(p_response->body);
    ptree pt;
    try {
      read_json(ss, pt);
      auto status = pt.get<int>("status");
      if (status == 200) {
        cached_credential_.set(user, pass);
        do_after_auth();
        return;
      }
    } catch (ptree_error& e) {
    }

    do_write_407_unauthorized();
  });

}

void Session::handle_auth() {
}

void Session::do_after_auth() {
  if (strncasecmp(p_request_->method.c_str(), "connect", strlen("connect")) == 0) {
    //connect request
    p_connect_request_ = p_request_;
    do_connect_request();
  } else {
    //non-connect request
    guid_.generate();
    post_request();
    do_direct_request();
  }
}

//处理非connect请求
void Session::do_direct_request() {
  PROLOGUE;

  auto& url_parts = p_request_->uri_parts();
  if (url_parts.find("host") == url_parts.end() || url_parts.find("schema") == url_parts.end()) return;

  tcp::resolver::query query(url_parts["host"], url_parts.find("port") == url_parts.end() ? url_parts["schema"] : url_parts["port"]);

  resolver_.async_resolve(query,
      [this, self](const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator)
      {
        handle_resolve_for_direct_request(ec, resolve_iterator);
      }
  );
}

void Session::handle_resolve_for_direct_request(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator) {
  PROLOGUE;

  if (!ec) {
    p_socket2_ = std::make_shared<tcp::socket>(socket_.get_io_service());
    p_ssl_socket2_ = nullptr;
    auto endpoint = *resolve_iterator++;
    p_socket2_->async_connect(endpoint,
      [this, self, resolve_iterator](const boost::system::error_code& ec) {
        handle_connect_for_direct_request(ec, resolve_iterator);
      }
    );
  } else {
    cout << "resolve error!" << endl;
  }
}

void Session::handle_connect_for_direct_request(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator) {
  PROLOGUE;

  if (!ec) {
    //连接成功，发送报文
    packet2_out_ = p_request_->forward_proxy_packet();
    cout << endl << "***********************" << endl
         << "session #" << (intptr_t)this << " request for " << p_request_->uri << ":" << endl
         << packet2_out_ << endl << "***********************" << endl;

    boost::asio::async_write(*p_socket2_, boost::asio::buffer(packet2_out_.data(), packet2_out_.length()),
      [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        handle_write_for_direct_request(ec, bytes_transferred);
      }
    );
  } else if (resolve_iterator != tcp::resolver::iterator()) {
    boost::system::error_code error;
    handle_resolve_for_direct_request(error, resolve_iterator);
  }
}

void Session::handle_write_for_direct_request(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  PROLOGUE;

  if (!ec) {
    do_read_for_direct_request();
  }
}

void Session::do_read_for_direct_request() {
  PROLOGUE;

  p_socket2_->async_read_some(boost::asio::buffer(buffer2_),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
      handle_read_for_direct_request(ec, bytes_transferred);
    }
  );
}

void Session::handle_read_for_direct_request(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  PROLOGUE;

  if (!ec) {
    packet2_in_.append(buffer2_.data(), bytes_transferred);
    try {
      //cout << packet2_in_ << endl;
      p_response2_ = HttpResponse::parse_response(packet2_in_);
      filtered_response_ = p_response2_->to_string();
      cout << endl << "***********************" << endl
           << "session #" << (intptr_t)this << " response for " << p_request_->uri << ":" << endl
           << filtered_response_ << endl << "***********************" << endl;
      post_response();

      packet2_in_ = "";

      boost::asio::async_write(socket_, boost::asio::buffer(filtered_response_.data(), filtered_response_.length()),
        [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
          handle_response_for_direct_request(ec, bytes_transferred);
        }
      );
    } catch (ReaderException e) {
      switch (e) {
        case ReaderException::AG:
        {
          do_read_for_direct_request();
          break;
        }
        case ReaderException::NG:
        {
          break;
        }
      }
    }
  }
}

void Session::handle_response_for_direct_request(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  PROLOGUE;

  if (!ec) {
    if (packet_in_.length() > 0) {
      boost::system::error_code error;
      handle_read_http_request(error, 0);
    } else {
      do_read_http_request();
    }
  }
}

void Session::do_connect_request() {
  PROLOGUE;

  auto& url_parts = p_request_->uri_parts();
  if (url_parts.find("host") == url_parts.end() || url_parts.find("port") == url_parts.end()) return;

  tcp::resolver::query query(url_parts["host"], url_parts["port"]);
  //cout << url_parts["host"] + " : " + url_parts["port"] << endl;

  resolver_.async_resolve(query,
      [this, self](const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator)
      {
        handle_resolve_for_connect_request(ec, resolve_iterator);
      }
  );
}

void Session::handle_resolve_for_connect_request(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator) {
  PROLOGUE;

  if (!ec) {
    p_socket2_ = std::make_shared<tcp::socket>(socket_.get_io_service());
    auto endpoint = *resolve_iterator++;
    p_socket2_->async_connect(endpoint,
      [this, self, resolve_iterator](const boost::system::error_code& ec) {
        handle_connect_for_connect_request(ec, resolve_iterator);
      }
    );
  } else {
    cout << "resolve error!" << endl;
  }
}

void Session::handle_connect_for_connect_request(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator) {
  PROLOGUE;

  if (!ec) {
    boost::asio::async_write(socket_, boost::asio::buffer(HttpToken::CONNECTION_ESTABLISHED, strlen(HttpToken::CONNECTION_ESTABLISHED)),
      [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        do_read_for_connect_request(ec);
      }
    );
  } else if (resolve_iterator != tcp::resolver::iterator()) {
    boost::system::error_code error;
    handle_resolve_for_connect_request(error, resolve_iterator);
  } else {
    auto& uri_parts = p_connect_request_->uri_parts();
    cout << "failed to connect " + uri_parts["host"] + ":" + uri_parts["port"] << endl;
  }
}

void Session::do_read_for_connect_request(const boost::system::error_code& ec) {
  PROLOGUE;

  if (packet_in_.length()) return;

  auto& uri_parts = p_connect_request_->uri_parts();
  auto p_domain_context = Session::context_for_domain(uri_parts["host"]);
  if (!p_domain_context) return;

  p_ssl_socket_ = std::make_shared<ssl_socket>(socket_, *p_domain_context);
  p_ssl_socket2_ = std::make_shared<ssl_socket>(*p_socket2_, context_);
  p_ssl_socket_->async_handshake(boost::asio::ssl::stream_base::server, [this, self](const boost::system::error_code& ec) {
    handle_handshake_for_connect_request(ec);
  });
}

void Session::handle_handshake_for_connect_request(const boost::system::error_code& ec) {
  PROLOGUE;

  if (!ec) {
    p_ssl_socket2_->async_handshake(boost::asio::ssl::stream_base::client, [this, self](const boost::system::error_code& ec) {
      if (!ec) do_read_after_handshake_for_connect_request();
    });
  } else {
    cout << "handshake fail: " << ec.message() << endl;
  }
}

void Session::do_read_after_handshake_for_connect_request() {
  PROLOGUE;

  p_ssl_socket_->async_read_some(boost::asio::buffer(buffer_),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
      handle_read_after_handshake_for_connect_request(ec, bytes_transferred);
    }
  );
}

void Session::handle_read_after_handshake_for_connect_request(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  PROLOGUE;

  if (!ec) {
    packet_in_.append(buffer_.data(), bytes_transferred);
    if (packet_in_.length() > Session::MAX_REQUEST_PACKET_SIZE) return;

    try {
      size_t parsed_length;
      p_request_ = HttpRequest::parse_request(packet_in_, &parsed_length);
      guid_.generate();
      post_request();
      do_ssl_proxy_request_for_connect_request();

      //pipelined request
      packet_in_= packet_in_.substr(parsed_length);
    } catch (ReaderException e) {
      switch (e) {
        case ReaderException::AG:
        {
          do_read_after_handshake_for_connect_request();
          break;
        }
        case ReaderException::NG:
        {
          break;
        }
      }
    }
  }
}

void Session::do_ssl_proxy_request_for_connect_request() {
  PROLOGUE;

  packet2_out_ = p_request_->forward_proxy_packet();
  boost::asio::async_write(*p_ssl_socket2_, boost::asio::buffer(packet2_out_.data(), packet2_out_.length()),
      [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        handle_ssl_proxy_request_for_connect_request(ec, bytes_transferred);
      }
  );
}

void Session::handle_ssl_proxy_request_for_connect_request(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  PROLOGUE;

  if (!ec) {
    do_ssl_read_for_connect_request();
  }
}

void Session::do_ssl_read_for_connect_request() {
  PROLOGUE;

  p_ssl_socket2_->async_read_some(boost::asio::buffer(buffer2_),
    [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
      handle_ssl_read_for_connect_request(ec, bytes_transferred);
    });
}

void Session::handle_ssl_read_for_connect_request(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  PROLOGUE;

  if (!ec) {
    packet2_in_.append(buffer2_.data(), bytes_transferred);
    try {
      p_response2_ = HttpResponse::parse_response(packet2_in_);
      filtered_response_ = p_response2_->to_string();
      /*
      cout << "packet2_in_:" << endl;
      cout << packet2_in_ << endl;
      cout << "filtered_response_:" << endl;
      cout << filtered_response_ << endl;
      */
      cout << endl << "***********************" << endl
           << "session #" << (intptr_t)this << " response for https://" << p_connect_request_->uri + p_request_->uri << ":" << endl
           << filtered_response_ << endl << "***********************" << endl;

      post_response();
      packet2_in_ = "";
      boost::asio::async_write(*p_ssl_socket_, boost::asio::buffer(filtered_response_.data(), filtered_response_.length()),
        [this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
          handle_ssl_response_for_connect_request(ec, bytes_transferred);
        }
      );
    } catch (ReaderException e) {
      switch (e) {
        case ReaderException::AG:
        {
          do_ssl_read_for_connect_request();
          break;
        }
        case ReaderException::NG:
        {
          break;
        }
      }
    }
  }
}

void Session::handle_ssl_response_for_connect_request(const boost::system::error_code& ec, std::size_t bytes_transferred) {
  PROLOGUE;

  if (!ec) {
    do_read_after_handshake_for_connect_request();
  }
}

void Session::post_request() {
  cout << __func__ << endl;
  ptree pt;
  pt.put("type", "request");
  pt.put("uuid", guid_.to_string());
  pt.put("user", auth_user_);
  pt.put("method", p_request_->method);
  if (p_connect_request_) {
    auto& url_parts = p_connect_request_->uri_parts();
    pt.put("uri", string("https://") + url_parts["host"] + (url_parts["port"] == "443" ? "" : (":" + url_parts["port"])) + p_request_->uri);
  } else pt.put("uri", p_request_->uri);
  pt.put("packet", p_request_->forward_proxy_packet());
  pt.put("body", p_request_->body);
  std::ostringstream buf;
  write_json (buf, pt, false);
  auto json = buf.str();
  post_http_request(PACKET_POST_URL, json);
}

void Session::post_response() {
  cout << __func__ << endl;
  if (packet2_in_.length() > Session::MAX_RESPONSE_PACKET_SIZE) {
    cout << "response too large:" << packet2_in_.length() << endl;
    return;
  }

  ptree pt;
  pt.put("type", "response");
  pt.put("uuid", guid_.to_string());
  pt.put("packet", filtered_response_);
  pt.put("body", p_response2_->body);
  std::ostringstream buf;
  write_json (buf, pt, false);
  auto json = buf.str();
  //cout << json << endl;
  post_http_request(PACKET_POST_URL, json);
}

void Session::post_http_request(string url, string data) {
  cout << __func__ << endl;
  auto p_client = new AsyncHttpClient(url, data, "application/json", socket_.get_io_service(), resolver_);
  p_client->post([](bool ok, response_ptr p_response) {
    //if (ok) cout << p_response->body << endl;
    //cout << (ok ? "suc" : "fail") << endl;
  });
}

ssl_context_ptr Session::context_for_domain(string domain) {
  bool ok;
  auto cert_path = Cert::cert_for_domain(domain, Conf::CSR_KEY, Conf::ROOT_CERT, Conf::ROOT_KEY, ok);
  if (!ok) return ssl_context_ptr(nullptr);

  if (domain_context_.find(domain) != domain_context_.end()) return domain_context_[domain];

  auto p_context = std::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

  p_context->set_options(
      boost::asio::ssl::context::default_workarounds
      | boost::asio::ssl::context::no_sslv2
      | boost::asio::ssl::context::single_dh_use);
  p_context->use_certificate_chain_file(cert_path.c_str());
  p_context->use_private_key_file(Conf::CSR_KEY, boost::asio::ssl::context::pem);

  domain_context_[domain] = p_context;
  return p_context;
}
