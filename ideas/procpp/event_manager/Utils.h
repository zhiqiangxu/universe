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
};

