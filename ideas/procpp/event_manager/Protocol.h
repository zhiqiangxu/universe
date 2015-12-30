#pragma once
#include <string>

using namespace std;


class IProtocol
{
public:
	virtual string read(int fd) = 0;
	virtual string read(int fd, size_t size) = 0;
	virtual string read(string& message, size_t size) = 0;
	virtual string readUtil(int fd, string separator) = 0;
	virtual string readUtil(string& message, string separator, int scanned/*调用方维护*/) = 0;
};

class Protocol : public IProtocol
{
public:
	virtual string read(int fd) override;//fd为ET
	virtual string read(int fd, size_t size) override;
	virtual string read(string& message, size_t size) override;
	virtual string readUtil(int fd, string separator) override;
	virtual string readUtil(string& message, string separator, int scanned = 0) override;
};
