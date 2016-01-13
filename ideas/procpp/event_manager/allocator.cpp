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

	auto pid = fork();

	//只有元素在shm
	vector<int, ShmAllocator<int>> v2;
	v2.push_back(20);

	if (pid) {
		//parent
		v[0] = 2;
		v2[0] = 30;

		sleep(3);

	} else {
		//child
		sleep(1);
		for (auto& i : v) {
			cout << i << endl;
		}
		for (auto& i : v2) {
			cout << i << endl;
		}

	}

	return 0;
}
