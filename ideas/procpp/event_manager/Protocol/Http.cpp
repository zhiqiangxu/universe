#include "Protocol/Http.h"
#include "ReactHandler.h"
#include <strings.h>//strncasecmp

void Http::on_connect(int client)
{
}

void Http::on_message(int client, string message)
{
	append_buf(client, message);

	StreamReader r(message);

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
		string method;
		r.read_until(" ", method);
		r.read_up(" ");

		// Request-URI    = "*" | absoluteURI | abs_path | authority
		string uri;
		r.read_until(" ", uri);
		r.read_up(" ");

		//HTTP-Version   = "HTTP" "/" 1*DIGIT "." 1*DIGIT
		string http_version;
		r.read_until("\r", http_version);

		char rn[2];
		r.read_size(sizeof(rn), rn);
		r.fail_if(strncmp(rn, "\r\n", 2) != 0);

/*
       message-header = field-name ":" [ field-value ]
       field-name     = token
       field-value    = *( field-content | LWS )
       field-content  = <the OCTETs making up the field-value
                        and consisting of either *TEXT or combinations
                        of token, separators, and quoted-string>
*/
		while (true) {
			char next_char;
cout << "test1" << endl;
			r.pread_plain(next_char);
cout << "test2" << endl;
			if (next_char == '\r') break;

cout << "before header_name" << endl;
			string header_name;
			r.read_until(":", header_name, true);

cout << "after header_name:" << header_name << endl;

			// LWS            = [CRLF] 1*( SP | HT )
			r.read_plain(next_char);
			switch(next_char) {
				case '\r':
				{
					r.read_plain(next_char);
					r.fail_if(next_char != '\n');
					r.read_up(" \t", false);
					break;
				}
				case ' ':
				case '\t':
				{
					r.read_up(" \t");
					break;
				}
				default: r.fail_if(true);
			
			}

cout << "after lws" << endl;

			//TODO support Transfer-Encoding
			r.fail_if(header_name == "Transfer-Encoding");

			//field-content
			//TODO 简化版，未严格对应
			string header_value;
			r.read_until("\r", header_value);

cout << "after header_value" << endl;
			cout << header_name << ": " << header_value << endl;

			r.read_size(sizeof(rn), rn);
			r.fail_if(strncmp(rn, "\r\n", 2) != 0);

		}
		r.read_size(sizeof(rn), rn);
		r.fail_if(strncmp(rn, "\r\n", 2) != 0);

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

	_server.write(client, "hello world!", sizeof("hello world!"));
	_server.close(client);
}

void Http::on_close(int client)
{
}
