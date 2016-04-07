#include "ReactHandler.h"
#include <iostream>

using namespace P::Client;
using namespace std;

int main()
{
	Client c;

	Redis proto(c);

	c.connect("localhost", 6379, [&proto, &c](int fd, ConnectResult r) {
		cout << "test" << endl;
		cout << Utils::enum_string(r) << endl;
		if (r == ConnectResult::OK) {
			proto.on_connect(fd);
			c.watch(fd, c.to_callbacks(proto));
		}
	}, true);

	c.start();
	return 0;
}
