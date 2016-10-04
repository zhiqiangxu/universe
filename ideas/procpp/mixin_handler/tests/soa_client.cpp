#include "ReactHandler.h"
using namespace std;


int main()
{
    // new style
    C::Soa c("localhost", 8082);


    GUID request_id;
    c.cmd<P::Client::Soa, P::Client::Soa::packet_type::JSON>(
        Utils::to_function([&request_id](string& json) {
            cout << "uuid = " << request_id.to_string() << " json = " << json << endl;
        }),
        request_id,
        string("[1,2,3]")
    );

    L.debug_log("before wait");
    c.wait(vector<GUID>({request_id}), 1500);
    L.debug_log("after wait");

    return 0;
}
