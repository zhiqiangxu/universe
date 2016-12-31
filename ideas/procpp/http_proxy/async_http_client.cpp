#include <iostream>
#include "async_http_client.h"
#include "utils.h"
#include "http.h"

using std::cout;
using std::endl;

AsyncHttpClient::AsyncHttpClient(const string& url, const string& data, const string& content_type, boost::asio::io_service& io_service, tcp::resolver& resolver)
: io_service_(io_service), socket_(io_service), resolver_(resolver), url_(url), data_(data), content_type_(content_type) {
}

void AsyncHttpClient::post(CB cb) {
  cb_ = cb;

  url_parts_ = Utils::parse_url(url_);

  tcp::resolver::query query(url_parts_["host"], url_parts_["schema"]);

  resolver_.async_resolve(query,
      [this](const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator)
      {
        handle_resolve(ec, resolve_iterator);
      }
  );
}

void AsyncHttpClient::handle_resolve(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator) {
  if (!ec) {
    auto endpoint = *resolve_iterator++;
    socket_.async_connect(endpoint,
      [this, resolve_iterator](const boost::system::error_code& ec) {
        handle_connect(ec, resolve_iterator);
      }
    );
  } else {
    done(false);
  }
}

void AsyncHttpClient::handle_connect(const boost::system::error_code& ec, tcp::resolver::iterator resolve_iterator) {
  //cout << __func__ << endl;
  if (!ec) {
    //连接成功，发送报文
    packet_ = HttpRequest::make_packet("POST", url_parts_, data_, content_type_);
    //cout << packet_ << endl;
    boost::asio::async_write(socket_, boost::asio::buffer(packet_.data(), packet_.length()),
      [this](const boost::system::error_code& ec, std::size_t bytes_transferred) {
        if (ec) {
          done(false);
          return;
        }
        do_read();
      }
    );
  } else if (resolve_iterator != tcp::resolver::iterator()) {
    boost::system::error_code error;
    handle_resolve(error, resolve_iterator);
  } else {
    done(false);
  }
}

void AsyncHttpClient::do_read() {
  //cout << __func__ << endl;
  socket_.async_read_some(boost::asio::buffer(buffer_),
    [this](const boost::system::error_code& ec, std::size_t bytes_transferred)
    {
      handle_response(ec, bytes_transferred);
    });
}

void AsyncHttpClient::handle_response(const boost::system::error_code& ec, size_t bytes_transferred) {
  //cout << __func__ << endl;
  if (!ec) {
    packet_out_.append(buffer_.data(), bytes_transferred);

    try {
      auto p_response = HttpResponse::parse_response(packet_out_);
      //cout << packet_out_ << endl;
      done(true, p_response);
    } catch (ReaderException e) {
      switch (e) {
        case ReaderException::AG:
        {
          do_read();;
          break;
        }
        case ReaderException::NG:
        {
          done(false);
          break;
        }
      }
    }
  } else done(false);
}

void AsyncHttpClient::done(bool suc, response_ptr p_response) {
  cb_(suc, p_response);
  delete this;
}
