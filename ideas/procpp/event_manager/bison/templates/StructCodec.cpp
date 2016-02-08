#include "StructCodec.h"

void {{ End }}::on_message(
	 int client, string message,
	 function<int client, void({{ ResultClass }} &r)> on_ok,
	 function<void(int client)> on_error,
	 function<void(int client)> on_again = []{}
					)
{
	append_buf(client, message);

	#include "generated_code.cpp"

}

void {{ ProtocolName }}::on_close(int client)
{
	erase_buf(client);
}


