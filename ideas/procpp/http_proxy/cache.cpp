#include "cache.h"

void Cache::set(const string& key, const string& value, int expire) {
  auto t = std::time(nullptr);
  cache_[key] = value;
  expire_[key] = t + expire;
}

bool Cache::get(const string& key, string& value) {
  if (expire_.find(key) == expire_.end()) return false;

  auto expire = expire_[key];
  auto t = std::time(nullptr);
  if (expire < t) {
    cache_.erase(key);
    expire_.erase(key);
    return false;
  }

  cache_[key] = value;
  return true;
}
