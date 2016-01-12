#include <iostream>
#include <vector>
#include "ShmAllocator.h"
#include <unistd.h>//sleep,fork

using namespace std;

int main()
{
	ShmAllocator<std::vector<int, ShmAllocator<int>>> al;

	auto vp = al.allocate(1);
	al.construct(vp);

	auto& v = *vp;

	auto pid = fork();

	if (pid) {
		//parent
		sleep(1);
		for (auto i : v) {
			cout << i << endl;
		}
	} else {
		//child
		v.push_back(1);

		sleep(3);
	}

	return 0;
}
