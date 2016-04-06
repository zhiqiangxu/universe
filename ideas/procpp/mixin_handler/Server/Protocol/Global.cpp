#include "Server/Protocol/Global.h"
#include "ReactHandler.h"

void Global::on_connect(int child)
{
	L.debug_log("global on_connect");
}

void Global::on_message(int child, string message)
{
	L.debug_log("global on_message");

	auto& server = dynamic_cast<ClientServer&>(_scheduler);

	server.append_buf(child, message);

	auto message_copy = message;//TODO optimize


	uint32_t total_size_n;
	if ( need_buf(child, message, !Protocol::read(message, total_size_n)) ) {
		return;
	}
	auto total_size = ntohl(total_size_n);
	if ( need_buf(child, message, message.length() < total_size) ) {
		return;
	}
	uint32_t session_id_n;
	Protocol::read(message, session_id_n);
	auto session_id = ntohl(session_id_n);

	server.write_global(session_id, message.substr(sizeof(uint32_t) + sizeof(uint32_t)).data(), total_size - sizeof(uint32_t) - sizeof(uint32_t));
	
	if (message_copy.length() > total_size) {
		on_message(child, message_copy.substr(total_size));
	}
}

void Global::on_close(int child)
{
	L.debug_log("global on_close");
}

