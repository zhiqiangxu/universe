#include "ReactHandler.h"

int main()
{
    L.debug_log(Utils::gzip_file("test.cpp", "test2.cpp") ? "OK" : "NG");

    return 0;
}
