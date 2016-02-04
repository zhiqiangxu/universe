#include "StructCodec.h"

ParseResult {{ End }}::on_message(int client, string message, {{ ResultClass }} &r)
{
	append_buf(worker_client, message);

	#include "generated_code.cpp"

}

void {{ ProtocolName }}::on_close(int client)
{
	erase_buf(client);
}


