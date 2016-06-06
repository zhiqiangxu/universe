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
#include <netdb.h>//getnameinfo
#include <arpa/inet.h>//inet_pton
#include <endian.h>    // __BYTE_ORDER
#include <algorithm>   // std::reverse
#include <chrono>//chrono::system_clock::now
#include <sys/types.h>//getpid
#include <unistd.h>
#include <uuid/uuid.h>//uuid_t
#include <sys/select.h>//timeval

using namespace std;

//put various static method here

enum class Endian
{
	Big,
	Little
};

enum class Encoding
{
	Utf8,
	Utf16LE,
	Utf16BE,
	Utf32LE,
	Utf32BE,
};

class GUID
{
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

	static pid_t getpid()
	{
		return ::getpid();
	}

	static void to_timespec(float sec, struct timespec& tv)
	{
		time_t seconds_num = static_cast<time_t>(sec);
		tv.tv_sec = seconds_num;
		tv.tv_nsec = (sec - seconds_num) * 1000000000/*9...*/;
	}

	static struct timeval to_timeval(int milliseconds)
	{
		struct timeval v;
		v.tv_sec = milliseconds/1000;
		v.tv_usec = (milliseconds % 1000) * 1000;

		return v;
	}

	static auto now()
	{
		return chrono::system_clock::now();
	}

	static struct sockaddr_un addr_sun(const string& sun_path)
	{
		struct sockaddr_un serveraddr;
		::bzero(&serveraddr, sizeof(serveraddr));
		serveraddr.sun_family = AF_UNIX;
		memcpy(serveraddr.sun_path, sun_path.c_str(), min(static_cast<int>(sun_path.length() + 1), UNIX_PATH_MAX));

		return serveraddr;
	}

	static struct sockaddr_in addr4(uint16_t port)
	{
		struct sockaddr_in serveraddr;
		::bzero(&serveraddr, sizeof(serveraddr));
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
		serveraddr.sin_port = htons(port);

		return serveraddr;
	}

	static struct sockaddr_in6 addr6(uint16_t port)
	{
		struct sockaddr_in6 serveraddr;
		::bzero(&serveraddr, sizeof(serveraddr));
		serveraddr.sin6_family = AF_INET6;
		serveraddr.sin6_port = htons(port);
		serveraddr.sin6_addr = in6addr_any;

		return serveraddr;
	}

	template <typename T>
	static void bzero(T& s)
	{
		::bzero(&s, sizeof(T));
	}

	static void bzero(void *s, size_t n)
	{
		::bzero(s, n);
	}

	template <typename T>
	static void* memcpy(T& dest, const T& src)
	{
		return ::memcpy(&dest, &src, sizeof(T));
	}

	static void* memcpy(void *dest, const void *src, size_t n)
	{
		return ::memcpy(dest, src, n);
	}

	union SocketAddress
	{
		struct sockaddr_in6 in6;
		struct sockaddr_in in4;
		struct sockaddr_un un;
	};

	static ssize_t addr_size(int domain)
	{
		if (domain == AF_INET) return sizeof(struct sockaddr_in);

		if (domain == AF_INET6) return sizeof(struct sockaddr_in6);

		if (domain == AF_UNIX) return sizeof(struct sockaddr_un);

		return -1;
	}

	static ssize_t addr_size(const SocketAddress& addr)
	{
		return addr_size(addr.in6.sin6_family);
	}

	static SocketAddress to_addr(const string& sun_path)
	{
		SocketAddress addr;
		bzero(addr);
		addr.un = addr_sun(sun_path);

		return addr;
	}

	static SocketAddress to_addr(const string& ip, uint16_t port)
	{
		SocketAddress addr;
		bzero(addr);

		if ( ip.find(":") != string::npos ) {
			addr.in6.sin6_family = AF_INET6;
			addr.in6.sin6_port = htons(port);

			inet_pton( AF_INET6, ip.c_str(), &(addr.in6.sin6_addr) );
		} else {
			addr.in4.sin_family = AF_INET;
			addr.in4.sin_port = htons(port);

			inet_pton( AF_INET, ip.c_str(), &(addr.in4.sin_addr) );
		}


		return addr;
	}

	static bool get_peer_name(int sock, SocketAddress& addr)
	{
		socklen_t len = sizeof(SocketAddress);
		auto ret = getpeername(sock, reinterpret_cast<struct sockaddr*>(&addr), &len);
	
		if (ret == -1) return false;

		return true;
	}

	static string get_name_info(const SocketAddress& addr);
	static string get_name_info(const struct sockaddr* sa);

	static void set_nonblock(int fd);
	static void set_keepalive(int socketfd, int keepidle = 3600, int keepinterval = 60, int keepcount = 5);
	static int nonblock_socket(int domain, int type, int protocol);

	static bool file_exists(const string& path)
	{
		struct stat buffer;
		return stat (path.c_str(), &buffer) == 0;
	}

	//https://en.wikipedia.org/wiki/ANSI_escape_code
	static string RED(const string& s)
	{
		return "\033[1;31m" + s + "\033[0m";
	}

	static string GREEN(const string& s)
	{
		return "\033[1;32m" + s + "\033[0m";
	}

	static string YELLOW(const string& s)
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

	static Endian get_endian()
	{
#if __BYTE_ORDER == __LITTLE_ENDIAN
		return Endian::Little;
#else
		return Endian::Big;
#endif
	}

	static Endian get_opposite_endian()
	{
#if __BYTE_ORDER != __LITTLE_ENDIAN
		return Endian::Little;
#else
		return Endian::Big;
#endif
	}

	template <typename T>
	static void swap_endian( T& value )
	{
		if (sizeof(T) == 1) return;

#if __BYTE_ORDER == __LITTLE_ENDIAN
		Utils::hton(value);
#else
		Utils::htol(value);
#endif
	}

	template <typename T, typename... R>
	static void print_types()
	{
		cout << typeid(T).name() << endl;
		if (sizeof...(R) > 0) print_types<R...>();
	}

	/**
	 *  It a SFINAE trick. If sizeof...(B) is true, enable_if::type will be void.
	 *  Otherwise, enable_if will not have any member named type, and template argument substitution will fail.
	 **/
	template <typename... T>
	static typename std::enable_if<sizeof...(T) == 0>::type print_types()
	{
	}


    //hacks from http://meh.schizofreni.co/programming/magic/2013/01/23/function-pointer-from-lambda.html

    template <typename Function>
    struct function_traits
      : public function_traits<decltype(&Function::operator())>
    {};

    template <typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...) const>
    {
      typedef ReturnType (*pointer)(Args...);
      typedef std::function<ReturnType(Args...)> function;
    };

    template <typename Function>
    static typename function_traits<Function>::function
    to_function (Function&& lambda)
    {
      return static_cast<typename function_traits<Function>::function>(lambda);
    }

	static string sha1(const string& data);

	static string rand_string(size_t length);

	static string string2hex(const string& s, bool space = false);
	static string string2hex(const char* s, size_t length, bool space = false);

    static string file_get_contents(const string& path);

    static bool gzip_file(const string& input_file, const string& output_file);
};



