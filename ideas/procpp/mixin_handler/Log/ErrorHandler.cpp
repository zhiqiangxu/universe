#include <stdlib.h>
#include <iostream>
#include "Log/ErrorHandler.h"
#include "Utils.h"

static ErrorHandler* _instance;

ErrorHandler& ErrorHandler::get_instance()
{
    if (!_instance) _instance = new ErrorHandler;

    return *_instance;

}


void ErrorHandler::error_log(const string& message)
{
    perror(message.c_str());
}

void ErrorHandler::error_exit(const string& message)
{
    perror(message.c_str());
    exit(1);
}

void ErrorHandler::debug_log(const string& message)
{
    cout << Utils::YELLOW(message) << endl;
}

void ErrorHandler::info_log(const string& message)
{
    cout << Utils::GREEN(message) << endl;
}

void ErrorHandler::assert(bool expression, const string& error_log)
{
    if (!expression) L.error_exit(error_log);
}
