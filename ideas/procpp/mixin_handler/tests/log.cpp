#include "ReactHandler.h"

int main()
{
    auto start = Utils::now();

    for (int i = 0; i < 10000; i++) {
        L.debug_log("this is a test purpose log...");
    }

    auto end = Utils::now();

    std::chrono::duration<double> elapsed = end - start;
    cout << "tooks " << elapsed.count() << "s\r\n";

}
