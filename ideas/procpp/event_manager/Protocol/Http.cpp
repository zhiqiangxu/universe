#include "Protocol/Http.h"
#include <strings.h>//strncasecmp

static void read_lws(StreamReader& r)
{
}

void Http::on_connect(int fd)
{
}

void Http::on_message(int fd, string message)
{
	append_buf(fd, message);

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
			r.pread_plain(next_char);
			if (next_char == '\r') break;

			string header_name;
			r.read_until(":", header_name);

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


		}
		r.read_size(sizeof(rn), rn);
		r.fail_if(strncmp(rn, "\r\n", 2) != 0);

	} catch (ReaderException e) {
	}
}

void Http::on_close(int fd)
{
}
