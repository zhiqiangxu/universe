#include <sstream>//std::istringstream
 #include <sys/types.h>//stat
 #include <sys/stat.h>//stat
 #include <unistd.h>//stat,unlink
#include <iomanip>//std::put_time
#include "utils.h"

using std::istringstream;

string GUID::to_string() {
  string result(37, 0);
  uuid_unparse( uuid, &result[0] );
  result.resize( 36 );

  return result;
}

bool GUID::from_string(const string& uuid_string, GUID& guid) {
  return uuid_parse( uuid_string.c_str(), guid.uuid ) == 0;
}

map<string, string> Utils::parse_url(const string& url) {
    map<string, string> result;

    auto idx = url.find("://");
    if (idx != string::npos) result["schema"] = url.substr(0, idx);

    auto next_idx = idx != string::npos ? idx + 3 : 0;

    idx = url.find('/', next_idx);
    if (idx == string::npos || idx > next_idx) {
        auto host_port = idx == string::npos ? url.substr(next_idx) : url.substr(next_idx, idx - next_idx);
        auto colon_idx = host_port.find(':');
        if (colon_idx != string::npos) {
            result["host"] = host_port.substr(0, colon_idx);
            result["port"] = host_port.substr(colon_idx + 1);
        } else {
            result["host"] = host_port;
        }
        next_idx = idx;
    }

    if (next_idx == string::npos) return result;

    idx = url.find_first_of("?#", next_idx);
    if (idx != string::npos) {
        result["path"] = url.substr(next_idx, idx - next_idx);

        next_idx = idx + 1;
        if (url[idx] == '?') {
            auto fragment_idx = url.find('#', next_idx);
            if (fragment_idx != string::npos) {
                result["query"] = url.substr(next_idx, fragment_idx - next_idx);
                next_idx = fragment_idx + 1;
            }
            else {
                result["query"] = url.substr(next_idx);
                next_idx = url.length();
            }
        }

        if (next_idx < url.length()) result["fragment"] = url.substr(next_idx);

    } else result["path"] = url.substr(next_idx);

    return result;
}

unsigned long Utils::hex2long(const string& hex) {
    unsigned long value;
    istringstream iss(hex);
    iss >> std::hex >> value;

    return value;
}

bool Utils::file_exists(const string& path) {
  struct stat buffer;
  return stat (path.c_str(), &buffer) == 0;
}

bool Utils::rm_file(const char *pathname) {
  return unlink(pathname) == 0;
}

string Utils::time_format(const char* format, std::time_t* p_time) {
  std::tm ltime;
  if (!p_time) {
    auto t = std::time(nullptr);
    ltime = *std::localtime(&t);
  } else ltime = *std::localtime(p_time);
  std::ostringstream oss;
  oss << std::put_time(&ltime, format);
  auto s = oss.str();
  return s;
}
