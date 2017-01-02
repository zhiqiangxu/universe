#pragma once
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <memory>//std::shared_ptr
#include "stream_reader.h"


using std::string;
using std::map;
using std::vector;
using std::pair;

class HttpRequest;
class HttpResponse;
using request_ptr = std::shared_ptr<HttpRequest>;
using response_ptr = std::shared_ptr<HttpResponse>;
using uri_parts_ptr = std::shared_ptr<map<string, string>>;

class HttpToken
{
  public:
    static constexpr const char* CONTENT_LENGTH = "Content-Length";
    static constexpr const char* CONNECTION = "Connection";
    //used by websocket
    static constexpr const char* SEC_WEBSOCKET_KEY = "Sec-WebSocket-Key";

    static constexpr const char* CONNECTION_ESTABLISHED = "HTTP/1.1 200 Connection established\r\n\r\n";
};

class HttpProviderAddress
{
  public:
    string host;
    int port;
    string path;
    string query;
    bool ssl;
};

class HttpRequest
{
  public:
    static request_ptr parse_request(const string& packet, size_t* parsed_length, size_t skip_length = 0);
    static string make_packet(const string& method, map<string, string>& url_parts, const string& data, const string& content_type);

    string method;
    string uri;
    string http_version;
    map<string, string> headers;
    string body;

    string forward_proxy_packet();
    map<string, string>& uri_parts();
    //bool should_keepalive();

  protected:
    uri_parts_ptr p_uri_parts_;
};

class HttpResponse
{
  public:
    static response_ptr parse_response(const string& s);

    int status_code;
    string reason_phrase;
    vector<pair<string, string>> headers;
    string body;

    string http_version;

    string to_string();

  private:
    static bool parse_chunked_body(StreamReader& s, HttpResponse& response);

};


