#include "functional"

#include "generated_code.h"

using namespace G{{ ProtocolName }};

class {{ End }} : public IStructCodec
{
public:

    // the standard permits lambda expressions in default parameters as long as nothing is captured
    void on_message(
                     int client, string message,
                     function<int client, void({{ ResultClass }} &r)> on_ok,
                     function<void(int client)> on_error,
                     function<void(int client)> on_again = []{}
                    );


    virtual void on_close(int client) override;

};


