
class ProtocolName : public IStructCodec
{
public:

	virtual ParseResult on_message(int client, string message) override;


	virtual void on_close(int client) override;

};


ParseResult ProtocolName::on_message(int client, string message)
{
	append_buf(worker_client, message);

	#include "generated_code.cpp"

}

void ProtocolName::on_close(int client)
{
	erase_buf(client);
}
