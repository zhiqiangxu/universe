#pragma once
#include <functional>
#include <vector>
#include <tuple>
#include <map>
#include <string>
#include "Utils.h"


using namespace std;

enum class EventType
{
	CONNECT,
	READ,
	WRITE,//一般不应该watch，除非write返回EAGAIN
	ERROR,
	CLOSE
};

enum class ConnectResult
{
	OK,
	NG,
	GAME_OVER,
};


class IEventManager
{
public:
	using CB = class U {
		public:
			using NR = function<void(int)>;
			using R = function<void(int, string)>;
			using C = function<void(int, ConnectResult)>;
			NR _nr;
			R _r;
			C _c;
			U() {}
			U(NR nr) : _nr(nr) {}
			U(R r) : _r(r) {}
			U(C c) : _c(c) {}
			void operator()(int fd) { _nr(fd); }
			void operator()(int fd, string message) { _r(fd, message); }
			void operator()(int fd, ConnectResult r) { _c(fd, r); }
			bool want_message() { return _r ? true : false; }
	};
	using EventCB = map<EventType, CB>;

	//如果同一个fd调两次，后者覆盖前者
	virtual bool watch(int fd, EventType event, CB callback) = 0;
	//仅在未通过EventManager关闭fd时，re_watch为true
	virtual bool watch(int fd, EventCB&& callbacks, bool re_watch) = 0;
	virtual bool unwatch(int fd, bool no_callback) = 0;
	/***异步close，并确保：如果有回调，只调用一次
		如fd已加入watch，则应该调用该方法关闭，否则callback无效
	****/
	virtual bool close(int fd, bool force_close) = 0;
	virtual bool close_all() = 0;
	virtual ssize_t write(int fd, const void *buf, size_t count) = 0;
	virtual ssize_t write_line(int fd, const string message) = 0;
	virtual ssize_t sendto(int u_sock, const void *buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen) = 0;
	virtual void start() = 0;
	virtual size_t count() = 0;
};

class EventManager : public IEventManager
{
public:
	EventManager();
	virtual ~EventManager() { _destroy(); };

	virtual bool watch(int fd, EventType event, CB callback) override;//TODO re_watch
    //TODO finish reading:
    //     http://www.codesynthesis.com/~boris/blog/2012/06/19/efficient-argument-passing-cxx11-part1/

	/***********************************perfect forwarding***************************************/
	/**http://stackoverflow.com/questions/3582001/advantages-of-using-forward/3582313#3582313****/
	/*************has the disadvantage that it can NOT be virtual********************************/
	/*************which makes the code ugly, so only do it when need*****************************/

	//use rvalue reference whenever possible is the BEST practice
	virtual bool watch(int fd, EventCB&& callbacks, bool re_watch = false) override;
	virtual bool unwatch(int fd, bool no_callback = false) override;
	virtual bool close(int fd, bool force_close = false) override;
	virtual bool close_all() override;
	virtual ssize_t write(int fd, const void *buf, size_t count) override;//TODO 完善，目前仅处理EPIPE
	virtual ssize_t write_line(int fd, const string message) override;
	virtual ssize_t sendto(int u_sock, const void *buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen) override;
	virtual void start() override;
	virtual size_t count() override;


private:
	bool _epoll_update(int fd, int epoll_op);
	void _add_close_fd(int fd);
	//called by dtor, works with fork
	virtual bool _destroy();//TODO almost perfect

	int _epoll_fd;
	vector<int> _close_fds;
	map<int, EventCB> _fds;

	int _current_fd = -1;
	EventCB _current_cb;
};

