#include "ReactHandler.h"

int main()
{
    L.debug_log(Utils::gzip_file("test.cpp", "test2.cpp") ? "OK" : "NG");

    auto parts = Utils::parse_url("ws://localhost:8082/abc?a=2#fragment");
    for (auto const& r : parts) {
        L.debug_log(r.first + " = " + r.second);
    }
    return 0;
}
