#pragma once
#include <thread>//hardware_concurrency
#include <iostream>//cout
#include <strings.h>//bzero
#include <string.h>//memcpy
#include <netinet/in.h>//sockaddr_in
#include <linux/un.h>//struct sockaddr_un
//stat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <type_traits>//underlying_type
#include <sys/socket.h>//getpeername

using namespace std;

//put various static method here

class Utils
{
public:
	static int get_cpu_cores()
	{
		auto n = thread::hardware_concurrency();

		if (n == 0) {
			cout << "hardware_concurrency returns 0" << endl;
			n = 8;
		}

		return n;
	}

	static struct sockaddr_un addr_sun(string sun_path)
	{
		struct sockaddr_un serveraddr;
		bzero(&serveraddr, sizeof(serveraddr));
		serveraddr.sun_family = AF_UNIX;
		memcpy(serveraddr.sun_path, sun_path.c_str(), min(static_cast<int>(sun_path.length() + 1), UNIX_PATH_MAX));

		return serveraddr;
	}

	static struct sockaddr_in addr4(uint16_t port)
	{
		struct sockaddr_in serveraddr;
		bzero(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons(port);

		return serveraddr;
	}

	static struct sockaddr_in6 addr6(uint16_t port)
	{
		struct sockaddr_in6 serveraddr;
		bzero(&serveraddr, sizeof(serveraddr));
		serveraddr.sin6_family = AF_INET6;
		serveraddr.sin6_port = htons(port);
		serveraddr.sin6_addr = in6addr_any;

		return serveraddr;
	}

	union SocketAddress
	{
		struct sockaddr_in6 in6;
		struct sockaddr_in in4;
		struct sockaddr_un un;
	};

	static bool get_peer_name(int socket, SocketAddress& addr)
	{
		socklen_t len = sizeof(SocketAddress);
		auto ret = getpeername(socket, reinterpret_cast<struct sockaddr*>(&addr), &len);
	
		if (ret == -1) return false;

		return true;
	}

	static bool file_exists(string path)
	{
		struct stat buffer;
		return stat (path.c_str(), &buffer) == 0;
	}

	//https://en.wikipedia.org/wiki/ANSI_escape_code
	static string RED(string s)
	{
		return "\033[1;31m" + s + "\033[0m";
	}

	static string GREEN(string s)
	{
		return "\033[1;32m" + s + "\033[0m";
	}

	static string YELLOW(string s)
	{
		return "\033[1;33m" + s + "\033[0m";
	}

	template<typename E>
	static auto enum_int(E e)
	{
		return static_cast<typename underlying_type<E>::type>(e);
	}

	template<typename E>
	static string enum_string(E e)
	{
		return enum_strings<E>::data[Utils::enum_int(e)];
	}

	// Ehis is the type that will hold all the strings.
	// Each enumerate type will declare its own specialization.
	// Any enum that does not have a specialization will generate a compiler error
	// indicating that there is no definition of this variable (as there should be
	// be no definition of a generic version).
	template<typename E>
	struct enum_strings
	{
		static const char * data[];
	};



};



