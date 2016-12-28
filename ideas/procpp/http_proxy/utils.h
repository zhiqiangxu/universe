#pragma once
#include <map>//std::map
#include <algorithm>//std::reverse
#include <uuid/uuid.h>//uuid_t
#include <unistd.h>//unlink
#include <chrono>//chrono::system_clock::now
#include <ctime>//time_t


using std::map;
using std::string;

class GUID {
    uuid_t uuid;
public:
    GUID() {};//uninitialized GUID
    GUID(const uuid_t& uuid) { uuid_copy(this->uuid, uuid); }
    GUID(const GUID& other) { uuid_copy(this->uuid, other.uuid); }
    void generate() { uuid_generate(uuid); }
    string to_string();
    static bool from_string(const string& uuid_string, GUID& guid);
    bool operator<(const GUID& other) const/*const is a MUST: http://stackoverflow.com/questions/4421706/operator-overloading*/ { return uuid_compare(uuid, other.uuid) < 0; }
};


class Utils {
  public:
    static map<string, string> parse_url(const string& url);
    static unsigned long hex2long(const string& hex);
    static bool file_exists(const string& path);
    static bool rm_file(const char *pathname);
    static string time_format(const char* format, std::time_t* p_time = nullptr);

    //network byte order is big endian
    // http://stackoverflow.com/a/28364285
    template <typename T>
    static void hton( T& value, char* ptr=0 )
    {
      if (sizeof(T) == 1) return;

#if __BYTE_ORDER == __LITTLE_ENDIAN
       ptr = reinterpret_cast<char*>(& value);
       std::reverse( ptr, ptr + sizeof(T) );
#endif
    }

    template <typename T>
    static void htol( T& value, char* ptr=0 )
    {
        if (sizeof(T) == 1) return;

#if __BYTE_ORDER != __LITTLE_ENDIAN
         ptr = reinterpret_cast<char*>(& value);
         std::reverse( ptr, ptr + sizeof(T) );
#endif
    }

    static bool is_big_endian()
    {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        return false;
#else
        return true;
#endif
    }

    static bool is_little_endian()
    {
#if __BYTE_ORDER == __LITTLE_ENDIAN
        return true;
#else
        return false;
#endif
    }

};
