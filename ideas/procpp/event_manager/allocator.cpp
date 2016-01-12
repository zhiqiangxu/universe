#include <iostream>
#include <vector>
#include "ShmAllocator.h"
using namespace std;

int main()
{
	std::vector<int, ShmAllocator<int> > v;
	v.push_back(1);

	for (auto i : v) {
		cout << i << endl;
	}
	return 0;
}
