#pragma once
#include <string>
using namespace std;

#define NEED_POP 0
#ifdef assert

    #pragma push_macro("assert")
    #undef assert
    #undef NEED_POP
    #define NEED_POP 1

#endif

#define DEBUG(exp, message) {\
	if (!(exp)) L.error_exit(message); \
}


#define L ErrorHandler::get_instance()


class IErrorHandler
{
public:
	virtual void error_log(const string& message) = 0;
	virtual void error_exit(const string& message) = 0;
	virtual void debug_log(const string& message) = 0;
	virtual void info_log(const string& message) = 0;
    virtual void assert(bool expression, const string& error_log) = 0;
};


class ErrorHandler : public IErrorHandler
{
public:
	static ErrorHandler& get_instance();
	virtual void error_log(const string& message) override;
	virtual void error_exit(const string& message) override;
	virtual void debug_log(const string& message) override;
	virtual void info_log(const string& message) override;
    virtual void assert(bool expression, const string& error_log) override;

	ErrorHandler(const ErrorHandler&) = delete;
	void operator=(const ErrorHandler&) = delete;
private:
	ErrorHandler() {};
	
};

#if NEED_POP
    #pragma pop_macro("assert")
#endif

#undef NEED_POP
