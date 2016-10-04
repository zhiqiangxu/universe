#include <iostream>
#include <vector>
#include "Allocator/ShmAllocator.h"
#include <unistd.h>//sleep,fork

using namespace std;

int main()
{
    //容器和元素都在shm
    ShmAllocator<vector<int, ShmAllocator<int>>> al;

    auto vp = al.allocate(1);
    al.construct(vp);

    auto& v = *vp;
    v.push_back(1);

    //只有元素在shm
    cout << "\n******************v2 start*******************\n" << endl;
    vector<int, ShmAllocator<int>> v2;
    v2.push_back(20);

    auto pid = fork();


    if (pid) {
        //parent
        cout << "parent enter" << endl;
        v[0] = 2;
        v2[0] = 30;

        sleep(3);
        cout << "parent exit" << endl;

    } else {
        //child
        sleep(1);
        cout << "child enter" << endl;
        for (auto& i : v) {
            cout << i << endl;
        }
        for (auto& i : v2) {
            cout << i << endl;
        }
        cout << "child exit" << endl;

    }

    return 0;
}
