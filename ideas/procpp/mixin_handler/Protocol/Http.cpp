#include "ReactHandler.h"
#include <string>

string HttpResponse::to_string()
{
    string s = http_version + " " + std::to_string(status_code) + " " + reason_phrase + "\r\n";
    for (const auto& h : headers) {
        if (h.first == HttpToken::CONTENT_LENGTH) continue;

        s += h.first + ": " + h.second + "\r\n";
    }
    s += string(HttpToken::CONTENT_LENGTH) + ": " + std::to_string(body.length()) + "\r\n";
    s += "\r\n";
    s += body;

    return s;
}


HttpResponse::HttpResponse(const HttpRequest& r)
    : status_code(200), reason_phrase("OK"), http_version(r.http_version)
{
}

string HttpRequest::forward_packet(const HttpProviderAddress& target_address)
{

    auto parts = Utils::parse_url(uri);
    parts["path"] = target_address.path;

    auto request_uri = parts["path"];
    auto query = target_address.query;
    if (parts.find("query") != parts.end()) query = Utils::merge_url_query(parts["query"], query);
    if (query.length()) request_uri += string("?") + query;

    auto host_port = target_address.host;
    //if (target_address.port != 80) host_port += ":" + to_string(target_address.port);

    string s = method + " " + request_uri + " " + http_version + "\r\n";
    bool has_host = false;
    for (const auto& h : headers) {
        if (h.first == "Host") {
            s += "Host: " + host_port + "\r\n";
            has_host = true;
        } else if (h.first == "Accept-Encoding") {
            continue;//TODO 支持编码
        } else s += h.first + ": " + h.second + "\r\n";
    }

    if (!has_host) s += "Host: " + host_port + "\r\n";
    s += "\r\n";
    s += body;

    return s;

}

bool HttpRequest::get_path(string& path)
{
    auto parts = Utils::parse_url(uri);
    if (parts.find("path") == parts.end()) return false;

    path = parts["path"];
    return true;
}



void Http::on_connect(int client)
{
}

HttpRequest Http::parse_request(int client, StreamReader& s)
{
    HttpRequest r;
    r.client = client;

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

    return r;
}

bool Http::parse_chunked_body(StreamReader& s, HttpResponse& response)
{

    char crlf[2];

    L.debug_log("start parse_chunked_body");
    while (true) {
        string length_string;
        s.read_until("\r", length_string);
        auto length = Utils::hex2long(length_string);

        s.read_size(sizeof(crlf), crlf);
        s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

        // TODO 此处依赖Accept-Encoding为空，也就是response不编码
        if (length > 0) s.read_size(length, response.body);
        s.read_size(sizeof(crlf), crlf);
        s.fail_if(strncmp(crlf, "\r\n", 2) != 0);

        if (length == 0) break;
    }

    return true;

}

void Http::on_message(int client, string message)
{

    append_buf(client, message);

    //L.debug_log(message);

    StreamReader s(message);


    size_t offset = 0;
    try {

        do {
            auto r = parse_request(client, s);
            offset = s.offset();

            HttpResponse resp(r);
            _scheduler.fire<ON_REQUEST, decltype(r)&, decltype(resp)&>(r, resp);

            auto output = resp.to_string();

            _scheduler.write(client, output.data(), output.length());

            if (close_if_necessary(r)) return;

        } while (!s.end());

    } catch (ReaderException e) {
        switch (e) {
            case ReaderException::AG:
            {
                need_buf(client, offset > 0 ? message.substr(offset) : message, true);
                return;
            }
            case ReaderException::NG:
            {
                _scheduler.close(client);
                return;
            }
        }
    }

}

void Http::on_close(int client)
{
    L.debug_log("client " + to_string(client) + " closed");
    erase_buf(client);
}

bool Http::close_if_necessary(HttpRequest& r)
{
    auto it = r.headers.find(HttpToken::CONNECTION);
    //close if required
    if (it != r.headers.end() && it->second == "close") {
        _scheduler.close(r.client);
        return true;
    }
    //default close if 1.0
    if (it == r.headers.end() && r.http_version.substr(sizeof("HTTP/") - 1) == "1.0") {
        _scheduler.close(r.client);
        return true;
    }

    return false;
}
