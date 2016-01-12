#include <iostream>
#include <vector>
#include "Allocator/ShmAllocator.h"
#include <unistd.h>//sleep,fork

using namespace std;

int main()
{
	ShmAllocator<vector<int, ShmAllocator<int>>> al;

	auto vp = al.allocate(1);
	al.construct(vp);

	auto& v = *vp;
	v.push_back(1);

	auto pid = fork();

	if (pid) {
		//parent
		v[0] = 2;

		sleep(3);

	} else {
		//child
		sleep(1);
		for (auto& i : v) {
			cout << i << endl;
		}

	}

	return 0;
}
