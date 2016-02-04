
#include "generated_code.h"

using namespace G{{ ProtocolName }};

class {{ End }} : public IStructCodec
{
public:

	ParseResult on_message(int client, string message, {{ ResultClass }} &r);


	virtual void on_close(int client) override;

};


