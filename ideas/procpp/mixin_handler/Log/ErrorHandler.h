#pragma once
#include <string>
using namespace std;

class IErrorHandler
{
public:
	virtual void error_log(const string& message) = 0;
	virtual void error_exit(const string& message) = 0;
	virtual void debug_log(const string& message) = 0;
	virtual void info_log(const string& message) = 0;
};


class ErrorHandler : public IErrorHandler
{
public:
	static ErrorHandler& get_instance();
	virtual void error_log(const string& message) override;
	virtual void error_exit(const string& message) override;
	virtual void debug_log(const string& message) override;
	virtual void info_log(const string& message) override;

	ErrorHandler(const ErrorHandler&) = delete;
	void operator=(const ErrorHandler&) = delete;
private:
	ErrorHandler() {};
	
};

