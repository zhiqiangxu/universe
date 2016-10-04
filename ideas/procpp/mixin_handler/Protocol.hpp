#include <string.h>


template <typename type>
bool Protocol::read(string& message, type& value)
{
    string result;
    if ( !Protocol::read(message, sizeof(type), result) ) return false;

    memcpy(&value, result.data(), sizeof(type));

    return true;
}


