#include "Utils.h"
#include "ReactHandler.h"
//setsockopt
#include <sys/types.h>
#include <sys/socket.h>
//IPPROTO_TCP
#include <netinet/in.h>
//TCP_KEEPIDLE
#include <netinet/tcp.h>
//fcntl
#include <unistd.h>
#include <fcntl.h>
#include <openssl/sha.h>//SHA1
#include <stdlib.h>//rand
#include <stdio.h>//snprintf

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
