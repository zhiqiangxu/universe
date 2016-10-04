#include "ReactHandler.h"
#include <map>
#include <iostream>
using namespace std;

int main()
{
    map<string, string> m;

    L.debug_log("prepare map");

    for (int i = 0; i < 10000000; i++) {
        m[Utils::rand_string(50)] = Utils::rand_string(20);
    }

    L.debug_log("look up map");

    auto start = Utils::now();
    for (int i = 0; i < 1000; i++) {
        auto f = m.find(Utils::rand_string(50));
    }
    auto end = Utils::now();
    auto elapsed_get = end - start;

    for (int i = 0; i < 1000; i++) {
        m[Utils::rand_string(50)] = Utils::rand_string(20);
    }

    auto elapsed_set = Utils::now() - end;

    cout << "1000 look up tooks " << elapsed_get.count() << "s\r\n";
    cout << "1000 set tooks " << elapsed_set.count() << "s\r\n";

    return 0;
}
