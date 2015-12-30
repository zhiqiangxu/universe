#include "Protocol.h"
#include <algorithm>//max
#include <unistd.h>//read

string Protocol::read(string& message, size_t size)
{
	//TODO error handle size

	if (message.length() < size) {
		return "";
	}

	auto result = message.substr(0, size);
	message = message.substr(size);

	return result;
}

string Protocol::readUtil(string& message, string separator, int scanned)
{
	auto pos = message.find(separator, max(0, static_cast<int>(scanned - separator.length())));
	if (pos != string::npos) {
		auto total_length = pos + separator.length();
		auto result = message.substr(0, total_length);
		message = message.substr(total_length);

		return result;
	}

	return "";
}

string Protocol::read(int fd)
{
	string message("");
	char buf[1024];

	while (true) {
		auto size = ::read(fd, buf, sizeof(buf));
		if (size == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return message;
			} else {
				//error handle
			}
			continue;
		} else if (size == 0) {
			//eof
			break;
		}

		// TODO 避免拷贝
		string sbuf("");
		sbuf.assign(buf, size);
		message += sbuf;
	}

	return message;
}

string Protocol::read(int fd, size_t size)
{
	return "no impl";
}

string Protocol::readUtil(int fd, string separator)
{
	return "no impl";
}
