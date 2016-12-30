#pragma once
#include <string>
#include <map>
#include <ctime>

using std::string;
using std::map;
using std::time_t;

class Cache {
  public:
    static const int DEFAULT_EXPIRE = 15*60;//默认15分钟

    void set(const string& key, const string& value, int expire = Cache::DEFAULT_EXPIRE);
    bool get(const string& key, string& value);

  protected:
    map<string, string> cache_;
    map<string, time_t> expire_;
};
