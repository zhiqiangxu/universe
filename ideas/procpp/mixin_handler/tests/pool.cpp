#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>
using namespace std;


int main()
{

	ThreadPool p;
	sleep(1);

	p.execute([] { cout << "hello world" << endl; });
	p.execute([] { cout << "hello world2" << endl; });

	sleep(10);
	return 0;
}
