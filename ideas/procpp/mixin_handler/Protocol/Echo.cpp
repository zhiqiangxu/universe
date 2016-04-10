#include "Protocol/Echo.h"
#include <iostream>//cout,endl
#include <unistd.h>//write
#include <string>
#include "ReactHandler.h"
using namespace std;



void Echo::on_message(int fd, string message)
{
	cout << "[read " + to_string(fd) + "]" << endl;
	cout << message;
	auto ignored = write(fd, message.data(), message.length());
}

void Echo::on_close(int fd)
{
	cout << "[closed " + to_string(fd) + "]" << endl;
}
