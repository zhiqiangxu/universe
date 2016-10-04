#include "Utils.h"
#include "ReactHandler.h"
//setsockopt
#include <sys/types.h>
#include <sys/stat.h>//umask
#include <sys/socket.h>
//IPPROTO_TCP
#include <netinet/in.h>
//TCP_KEEPIDLE
#include <netinet/tcp.h>
//fcntl
#include <unistd.h>//fork,setsid,chdir
#include <fcntl.h>
#include <openssl/sha.h>//SHA1
#include <stdlib.h>//rand,mkstemp,exit
#include <stdio.h>//snprintf,rename,fopen,freopen
#include <fstream>//ifstream
#include <sstream>//istringstream
#include <zlib.h>
#include <sys/types.h>
#include <sys/wait.h>//waitpid

template<>
const char* Utils::enum_strings<Encoding>::data[] = {"Utf8", "Utf16LE", "Utf16BE", "Utf32LE", "Utf32BE"};


string GUID::to_string()
{
    string result(37, 0);

    uuid_unparse( uuid, &result[0] );
    result.resize( 36 );

    return result;
}

bool GUID::from_string(const string& uuid_string, GUID& guid)
{
    return uuid_parse( uuid_string.c_str(), guid.uuid ) == 0;
}

string Utils::get_name_info(const SocketAddress& addr)
{
    auto addrp = reinterpret_cast<const struct sockaddr*>(&addr);
    return get_name_info(addrp);
}

string Utils::get_name_info(const struct sockaddr* sa)
{
    static char host[UNIX_PATH_MAX];
    static char service[UNIX_PATH_MAX];

    if (getnameinfo(sa, addr_size(sa->sa_family), host, sizeof(host), service, sizeof(service), 0) != 0) {
        L.error_exit("getnameinfo");
    }

    return string(host) + ":" + string(service);
}

void Utils::set_nonblock(int fd)
{
    auto old_flags = fcntl(fd, F_GETFL);

    if (old_flags & O_NONBLOCK) return;//已经nonblock

    auto new_flags = old_flags | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, new_flags) == -1) L.error_exit("set_nonblock");
}

void Utils::set_keepalive(int socketfd, int keepidle, int keepinterval, int keepcount)
{
    int sockopt = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, &sockopt, sizeof(int)) == -1) L.error_exit("setsockopt SO_KEEPALIVE");

    if (setsockopt(socketfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepidle, sizeof(int)) == -1) L.error_exit("setsockopt TCP_KEEPIDLE");

    if (setsockopt(socketfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepinterval, sizeof(int)) == -1) L.error_exit("setsockopt TCP_KEEPINTVL");

    if (setsockopt(socketfd, IPPROTO_TCP, TCP_KEEPCNT, &keepcount, sizeof(int)) == -1) L.error_exit("setsockopt TCP_KEEPCNT");
}

int Utils::nonblock_socket(int domain, int type, int protocol)
{
    auto s = socket(domain, type, protocol);
    if (s == -1) {
        L.error_log("socket");
        return -1;
    }

    set_nonblock(s);
    return s;
}

string Utils::sha1(const string& data)
{
    unsigned char h[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)data.data(), data.length(), h);

    return string((char*)h, SHA_DIGEST_LENGTH);
}

string Utils::rand_string(size_t length)
{
    static string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    string result;
    result.resize(length);

    for (size_t i = 0; i < length; i++)
        result[i] = charset[rand() % charset.length()];

    return result;
}

string Utils::string2hex(const string& s, bool space)
{
    string result;
    char buf[2];

    for (size_t i = 0; i < s.length(); i++) {
        sprintf(buf, "%02X", s[i]);
        result.append(buf, 2);
        if (space) result += ' ';
    }

    return result;
}

string Utils::string2hex(const char* s, size_t length, bool space)
{
    string result;
    char buf[3];

    for (size_t i = 0; i < length; i++) {
        snprintf(buf, sizeof(buf), "%02X", s[i]);//snprintf will always append \0 to buf
        result.append(buf, 2);
        if (space) result += ' ';
    }


    return result;
}

unsigned long Utils::hex2long(const string& hex)
{
    unsigned long value;
    istringstream iss(hex);
    iss >> std::hex >> value;

    return value;
}

string Utils::file_get_contents(const string& path)
{
    ifstream ifs(path);
    // it won't compile without parenthesis
    // it's the "classic" c++ parsing problem called Most Vexing Parse
    // http://stackoverflow.com/a/2912614
    string content( (istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()) );

    return content;
}

bool Utils::gzip_file(const string& input_file, const string& output_file)
{
    char tmp_path[] = "/tmp/template-XXXXXX";
    auto tmp_fd = mkstemp(tmp_path);
    close(tmp_fd);

    auto input = fopen(input_file.c_str(), "rb");
    if (input == nullptr) {
        L.error_log("failed to open " + input_file);
        return false;
    }

    auto output = gzopen(tmp_path, "wb");
    char buffer[8 * 1024];
    int n;
    while ((n = fread(buffer, 1, sizeof(buffer), input)) > 0) {
        gzwrite(output, buffer, n);
    }

    fclose(input);
    gzclose(output);

    rename(tmp_path, output_file.c_str());

    return true;
}

void Utils::supervise_subprocess(const function<void(void)>& child_callback)
{
    int status;
    pid_t child;
    while ((child = waitpid(-1, &status, 0)) > 0)
    {
        auto pid = fork();
        if (pid == -1) L.error_exit("fork");

        if (pid) {
        } else {

            child_callback();

        }
    }
}

//http://stackoverflow.com/questions/3095566/linux-daemonize
void Utils::daemonize(const char* std_out, const char* std_err, const char* std_in)
{
    auto pid = fork();
    L.assert(pid != -1, "fork 1");

    if (pid) exit(0);

    auto sid = setsid();
    L.assert(sid != -1, "setsid");

    pid = fork();
    if (pid == -1) L.error_exit("fork 2");
    L.assert(pid != -1, "fork 2");

    if (pid) exit(0);

    auto ret = chdir("/");

    L.assert(ret != -1, "chdir /");

    umask(0);

    if (std_out) L.assert(freopen(std_out, "w", stdout) != nullptr, "freopen stdout");
    if (std_err) L.assert(freopen(std_err, "w", stderr), "freopen stderr");
    if (std_in) L.assert(freopen(std_in, "r", stdin), "freopen stdin");
}

//TODO support user,pass
map<string, string> Utils::parse_url(const string& url)
{
    map<string, string> result;

    auto idx = url.find("://");
    if (idx != string::npos) result["schema"] = url.substr(0, idx);

    auto next_idx = idx != string::npos ? idx + 3 : 0;

    idx = url.find('/', next_idx);
    if (idx != string::npos && idx > next_idx) {
        auto host_port = url.substr(next_idx, idx - next_idx);
        auto colon_idx = host_port.find(':');
        if (colon_idx != string::npos) {
            result["host"] = host_port.substr(0, colon_idx);
            result["port"] = host_port.substr(colon_idx + 1);
        } else {
            result["host"] = host_port;
        }
        next_idx = idx;
    }

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

vector<string> Utils::split(const string& str, const string& delim)
{
    vector<string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == string::npos) pos = str.length();
        string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());

    return tokens;
}

string Utils::join(const string& glue, const vector<string>& pieces)
{
    if (pieces.size() == 0) return "";

    if (pieces.size() == 1) return pieces[0];

    auto result = pieces[0];

    for (size_t i = 1; i < pieces.size(); i++) result += glue + pieces[i];

    return result;
}

string Utils::merge_url_query(const string& query1, const string& query2)
{
    if (query2.length() == 0) return query1;

    auto tokens1 = Utils::split(query1, "&");
    map<string, string> query_map1;
    for (size_t i = 0; i < tokens1.size(); i++) {
        auto kv = Utils::split(tokens1[i], "=");
        query_map1[kv[0]] = kv.size() > 1 ? kv[1] : "";
    }
    auto tokens2 = Utils::split(query2, "&");
    map<string, string> query_map2;
    for (size_t i = 0; i < tokens2.size(); i++) {
        auto kv = Utils::split(tokens2[i], "=");
        query_map2[kv[0]] = kv.size() > 1 ? kv[1] : "";
    }

    for (auto& item : query_map2) {
        query_map1[item.first] = item.second;
    }

    vector<string> query;
    for (auto& item : query_map1) query.push_back(item.first + "=" + item.second);

    return Utils::join("&", query);
}

