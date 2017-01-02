#include <sstream>//std::istringstream
 #include <sys/types.h>//stat
 #include <sys/stat.h>//stat
 #include <unistd.h>//stat,unlink
#include <iomanip>//std::put_time
#include <regex>//regex_token_iterator
#include "utils.h"

using std::ostringstream;
using std::setw;
using std::hex;
using std::nouppercase;
using std::uppercase;
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

/*****BASE64 START*****/
static const char b64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char reverse_table[128] = {
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
   64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
   52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
   64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
   64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};


string Utils::base64_decode(const string& base64_value) {
  string retval;
  const auto last = base64_value.end();

  int bits_collected = 0;
  unsigned int accumulator = 0;

  for (auto i = base64_value.begin(); i != last; ++i)
  {
      const int c = *i;
      if (isspace(c) || c == '=') {
          // Skip whitespace and padding. Be liberal in what you accept.
          continue;
      }

      if ((c > 127) || (c < 0) || (reverse_table[c] > 63)) {
          //This contains characters not legal in a base64 encoded string.
          return "";
      }

      accumulator = (accumulator << 6) | reverse_table[c];
      bits_collected += 6;
      if (bits_collected >= 8) {
          bits_collected -= 8;
          retval += (char)((accumulator >> bits_collected) & 0xffu);
      }
  }

  return retval;
}

string Utils::base64_encode(const string& value) {
    const auto binlen = value.size();

    // Use = signs so the end is properly padded.
    string retval((((binlen + 2) / 3) * 4), '=');

    size_t outpos = 0;
    int bits_collected = 0;
    unsigned int accumulator = 0;
    const auto binend = value.end();

    for (auto i = value.begin(); i != binend; ++i)
    {
        accumulator = (accumulator << 8) | (*i & 0xffu);
        bits_collected += 8;
        while (bits_collected >= 6)
        {
            bits_collected -= 6;
            retval[outpos++] = b64_table[(accumulator >> bits_collected) & 0x3fu];
        }
    }

    if (bits_collected > 0) { // Any trailing bits that are missing.
        accumulator <<= 6 - bits_collected;
        retval[outpos++] = b64_table[accumulator & 0x3fu];
    }

    return retval;
}
/*****BASE64 END*****/


/*
std::vector<string> Utils::split(const string& input, const string& regex) {
  // passing -1 as the submatch index parameter performs splitting
  std::regex re(regex);
  std::sregex_token_iterator
      first{input.begin(), input.end(), re, -1},
      last;
  return {first, last};
}
*/

string Utils::url_encode(const string& value) {
  ostringstream escaped;
  escaped.fill('0');
  escaped << hex;

  for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
      string::value_type c = (*i);

      // Keep alphanumeric and other accepted characters intact
      if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
          escaped << c;
          continue;
      }

      // Any other characters are percent-encoded
      escaped << uppercase;
      escaped << '%' << setw(2) << int((unsigned char) c);
      escaped << nouppercase;
  }

  return escaped.str();
}
