#include "ReactHandler.h"
#include <iostream>

using namespace P::Client;
using namespace std;

int main()
{
	Client c;

	Redis proto(c);

	c.connect("localhost", 6379, [](int fd, ConnectResult r) {
		cout << "test" << endl;
		cout << Utils::enum_string(r) << endl;
	}, true);

	c.start();
	return 0;
}
