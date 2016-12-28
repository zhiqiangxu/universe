#include <iostream>
#include "http.h"
#include "stream_reader.h"
using std::cout;
using std::endl;


request_ptr HttpRequest::parse_request(const string& packet, size_t* parsed_length, size_t skip_length) {
  StreamReader s(packet, skip_length);

  auto request = std::make_shared<HttpRequest>();
  auto& r = *request;

  /*

  Request       = Request-Line              ; Section 5.1
                  *(( general-header        ; Section 4.5
                   | request-header         ; Section 5.3
                   | entity-header ) CRLF)  ; Section 7.1
                  CRLF
                  [ message-body ]
  */
  // Request-Line   = Method SP Request-URI SP HTTP-Version CRLF
  s.read_until(" ", r.method);
  s.read_up(" ");

  // Request-URI    = "*" | absoluteURI | abs_path | authority
  s.read_until(" ", r.uri);
  s.read_up(" ");

  //HTTP-Version   = "HTTP" "/" 1*DIGIT "." 1*DIGIT
  s.read_until("\r", r.http_version);

  char crlf[2];
  s.read_size(sizeof(crlf), crlf);
  s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

/*
 message-header = field-name ":" [ field-value ]
 field-name     = token
 field-value    = *( field-content | LWS )
 field-content  = <the OCTETs making up the field-value
                  and consisting of either *TEXT or combinations
                  of token, separators, and quoted-string>
*/
  int content_length = -1;
  while (true) {
      char next_char;
      s.pread_plain(next_char);
      if (next_char == '\r') break;

      string header_name;
      s.read_until(":", header_name, true);


      // LWS            = [CRLF] 1*( SP | HT )
      //TODO 简化版，未严格对应
      s.read_plain(next_char);
      s.fail_if(next_char != ' ');



      //TODO support Transfer-Encoding
      s.fail_if(header_name == "Transfer-Encoding");

      //field-content
      //TODO 简化版，未严格对应
      string header_value;
      s.read_until("\r", header_value);
      r.headers[header_name] = header_value;

      if (header_name == "Content-Length") content_length = atoi(header_value.c_str());

      s.read_size(sizeof(crlf), crlf);
      s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

  }
  s.read_size(sizeof(crlf), crlf);
  s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

  if (content_length > 0) {
      r.body.resize(content_length);
      s.read_size(content_length, &r.body[0]);
  }

  if (parsed_length) *parsed_length = s.offset() - skip_length;

  return request;
}

string HttpRequest::make_packet(const string& method, map<string, string>& url_parts, const string& data) {
  return method + " " + (url_parts.find("path") != url_parts.end() ? url_parts["path"] : "/") + " HTTP/1.1\r\n" \
        "Host: " + url_parts["host"] + "\r\n" \
        "Content-Length: " + std::to_string(data.length()) + "\r\n" \
        "User-Agent: proactor\r\n\r\n" + data;
}

string HttpRequest::forward_proxy_packet() {

  string s = method + " " + uri + " " + http_version + "\r\n";
  for (const auto& h : headers) {
    if (h.first == "Accept-Encoding") continue;//转发不支持编码
    s += h.first + ": " + h.second + "\r\n";
  }

  s += "\r\n";
  s += body;

  return s;
}

map<string, string>& HttpRequest::uri_parts() {
  if (!p_uri_parts_) p_uri_parts_ = std::make_shared<map<string, string>>(Utils::parse_url(uri));

  return *p_uri_parts_;
}

response_ptr HttpResponse::parse_response(const string& packet) {
  auto response = std::make_shared<HttpResponse>();
  auto&  r = *response;

  StreamReader s(packet);
  /*

 Response      = Status-Line               ; Section 6.1
                 *(( general-header        ; Section 4.5
                  | response-header        ; Section 6.2
                  | entity-header ) CRLF)  ; Section 7.1
                 CRLF
                 [ message-body ]          ; Section 7.2

  */
  // Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
  s.read_until(" ", r.http_version);
  s.read_up(" ");

  string status;
  s.read_until(" ", status);
  r.status_code = atoi(status.c_str());
  s.read_up(" ");

  s.read_until("\r", r.reason_phrase);

  char crlf[2];
  s.read_size(sizeof(crlf), crlf);
  s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

/*
 message-header = field-name ":" [ field-value ]
 field-name     = token
 field-value    = *( field-content | LWS )
 field-content  = <the OCTETs making up the field-value
                  and consisting of either *TEXT or combinations
                  of token, separators, and quoted-string>
*/
  int content_length = -1;
  bool is_chunked = false;
  while (true) {
      char next_char;
      s.pread_plain(next_char);
      if (next_char == '\r') break;

      string header_name;
      s.read_until(":", header_name, true);


      // LWS            = [CRLF] 1*( SP | HT )
      //TODO 简化版，未严格对应
      s.read_plain(next_char);
      s.fail_if(next_char != ' ');


      //field-content
      //TODO 简化版，未严格对应
      string header_value;
      s.read_until("\r", header_value);
      s.read_size(sizeof(crlf), crlf);
      s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

      // Transfer-Encoding只有chunked方式
      if (header_name == "Transfer-Encoding") {
          s.fail_if(header_value != "chunked");
          is_chunked = true;
          continue;
      }

      r.headers.push_back(make_pair(header_name, header_value));

      if (header_name == "Content-Length") content_length = atoi(header_value.c_str());

  }
  s.read_size(sizeof(crlf), crlf);
  s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

  if (content_length > 0) {
      r.body.resize(content_length);
      s.read_size(content_length, &r.body[0]);
  } else if (is_chunked) {
      parse_chunked_body(s, r);
  } else if (content_length == 0) {//do nothing
  } else if (content_length == -1 && s.end()) {//no chunk, no size, work around
    r.headers.push_back(make_pair("Content-Length", string("0")));
  } else s.fail_if(true);//Content-Length或者chunked或者都没有且body空

  return response;
}
  /*

 Response      = Status-Line               ; Section 6.1
                 *(( general-header        ; Section 4.5
                  | response-header        ; Section 6.2
                  | entity-header ) CRLF)  ; Section 7.1
                 CRLF
                 [ message-body ]          ; Section 7.2

  */
string HttpResponse::to_string() {
  string response = http_version + " " + std::to_string(status_code) + " " + reason_phrase + "\r\n";
  for (auto& header:headers) {
    if (header.first == "Strict-Transport-Security") continue;
    response += header.first + ": " + header.second + "\r\n";
  }
  response += "\r\n" + body;
  return response;
}

bool HttpResponse::parse_chunked_body(StreamReader& s, HttpResponse& response) {
  cout << __func__ << std::endl;
  char crlf[2];

  while (true) {
      string length_string;
      s.read_until("\r", length_string);
      auto length = Utils::hex2long(length_string);

      s.read_size(sizeof(crlf), crlf);
      s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

      // 此处依赖Accept-Encoding为空，也就是response不编码
      if (length > 0) s.read_size(length, response.body);
      s.read_size(sizeof(crlf), crlf);
      s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

      if (length == 0) break;
  }

  response.headers.push_back(make_pair("Content-Length", std::to_string(response.body.length())));

  return true;
}

