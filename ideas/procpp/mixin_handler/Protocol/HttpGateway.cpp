
#include "ReactHandler.h"
#include <stdlib.h>//atoi
#include <utility>//make_pair


void HttpGateway::on_connect(int client)
{
    set_state(client, HttpGatewayState::PARSE_REQUEST);
}

void HttpGateway::on_message(int client, string message)
{
    append_buf(client, message);

    HttpRequest r;
    try {

        auto state = get_state(client);
        // if pipeline, just buffer it
        if (state == HttpGatewayState::PARSE_RESPONSE) throw ReaderException::AG;

        StreamReader s(message);


        r = parse_request(client, s);
        auto offset = s.offset();

        need_buf(client, message.substr(offset), true);
        // no more request until responsed
        set_state(client, HttpGatewayState::PARSE_RESPONSE);

    } catch (ReaderException e) {
        switch (e) {
            case ReaderException::AG:
            {
                need_buf(client, message, true);
                return;
            }
            case ReaderException::NG:
            {
                _server.close(client);
                return;
            }
        }
    }

    HttpProviderAddress target_address;
    bool ok = false;

    _server.fire<ON_REQUEST_URI, HttpRequest&, decltype(target_address)&, bool&>(r, target_address, ok);

    if (!ok) {
        _server.close(client);
        return;
    }

    _forward_info[client] = make_pair(r, target_address);

    auto remote_fd = _server.connect(target_address.host, target_address.port, EventManager::EventCB{
        {
            EventType::CONNECT, EventManager::CB([this, client] (int remote_fd, ConnectResult r) {
                on_remote_connect(remote_fd, r, client);
            }),
        },
        {
            EventType::READ, EventManager::CB([this, client] (int remote_fd, string message) {
                on_remote_message(remote_fd, message, client);
            })
        },
        {
            EventType::CLOSE, EventManager::CB([this, client] (int remote_fd) {
                on_remote_close(remote_fd, client);
            })
        },

    }, true);


    if (remote_fd == -1) {
        L.error_log("remote_fd = -1");
        return;
    }

    _c2r[client] = remote_fd;
    _r2c[remote_fd] = client;

}

void HttpGateway::on_close(int client)
{
    Http::on_close(client);
    erase_state(client);

    if (_c2r.find(client) != _c2r.end()) {
        auto remote_fd = _c2r[client];

        _c2r.erase(client);

        close_remote(remote_fd);
    }
}

void HttpGateway::on_remote_connect(int remote_fd, ConnectResult r, int client)
{
    if (r != ConnectResult::OK) {
        _server.close(client);
        return;
    }

    auto& forward_info = _forward_info[client];
    auto& request = forward_info.first;
    auto& target_address = forward_info.second;
    _server.write(remote_fd, request.forward_packet(target_address));
}

void HttpGateway::on_remote_message(int remote_fd, string message, int client)
{
    append_buf(remote_fd, message);
    StreamReader s(message);
    HttpResponse r;

    try {
        r = parse_response(remote_fd, s);
    } catch (ReaderException e) {
        switch (e) {
            case ReaderException::AG:
            {
                need_buf(remote_fd, message, true);
                return;
            }
            case ReaderException::NG:
            {
                _server.close(remote_fd);
                return;
            }
        }
    }

    _server.write(client, r.to_string());
    if (close_if_necessary(_forward_info[client].first)) return;

    close_remote(remote_fd);//TODO reuse remote_fd


    set_state(client, HttpGatewayState::PARSE_REQUEST);

    //trigger buffered request
    if (has_buf(client)) on_message(client, get_buf(client));
}

void HttpGateway::on_remote_close(int remote_fd, int client)
{
    //如果不是主动关闭remote_fd,则关闭client
    if (_r2c.find(remote_fd) != _r2c.end()) _server.close(client);
    else _r2c.erase(remote_fd);

    erase_buf(remote_fd);
    _forward_info.erase(client);
}

void HttpGateway::close_remote(int remote_fd)
{
    erase_buf(remote_fd);
    _r2c.erase(remote_fd);
    _server.close(remote_fd);
}

HttpResponse HttpGateway::parse_response(int remote_fd, StreamReader& s)
{

	HttpResponse r;

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
		r.headers.push_back(make_pair(header_name, header_value));

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
