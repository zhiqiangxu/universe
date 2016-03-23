#include "Protocol/Http.h"
#include "ReactHandler.h"
#include <strings.h>//strncasecmp

string HttpResponse::to_string()
{
	string s = string(http_version) + " " + std::to_string(status_code) + " " + reason_phrase + "\r\n";
	for (const auto& h : headers) {
		s += h.first + ": " + h.second + "\r\n";
	}
	if (body.length() > 0 && headers.find(HttpToken::CONTENT_LENGTH) == headers.end()) {
		s += string(HttpToken::CONTENT_LENGTH) + ": " + std::to_string(body.length()) + "\r\n";
	}
	s += "\r\n";
	s += body;

	return s;
}

void Http::on_connect(int client)
{
}

void Http::on_message(int client, string message)
{

	append_buf(client, message);

	StreamReader s(message);

	HttpRequest r;
	r.client = client;
	try {

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
			r.body.reserve(content_length);
			s.read_size(content_length, &r.body[0]);
		}

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

	using Hook = EventHookGlobal<Http::ON_REQUEST, HttpRequest&, HttpResponse&>;

	HttpResponse resp;
	Hook::get_instance().fire(r, resp);

	resp.body = message + resp.body;
	auto output = resp.to_string();

	_server.write(client, output.data(), output.length());
	//_server.close(client);

}

void Http::on_close(int client)
{
	L.debug_log("client " + to_string(client) + " closed");
	erase_buf(client);
}
