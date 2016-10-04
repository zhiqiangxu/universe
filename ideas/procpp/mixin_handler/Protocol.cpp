#include <algorithm>//max
#include <unistd.h>//read
#include <string.h>//memcpy
#include "ReactHandler.h"

bool Protocol::read(string& message, size_t size, string& result)
{
    //TODO error handle size

    if (message.length() < size) {
        return false;
    }

    result = message.substr(0, size);
    message = message.substr(size);

    return true;
}

bool Protocol::read_until(string& message, string separator, string& result, int scanned)
{
    auto pos = message.find(separator, max(0, static_cast<int>(scanned - separator.length())));
    if (pos != string::npos) {
        auto total_length = pos + separator.length();
        result = message.substr(0, total_length);
        message = message.substr(total_length);

        return true;
    }

    return false;
}


