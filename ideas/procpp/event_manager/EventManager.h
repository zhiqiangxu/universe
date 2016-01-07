#pragma once
#include <functional>
#include <vector>
#include <tuple>
#include <map>
#include <string>


using namespace std;

enum class EventType
{
	CONNECT,//客户端用
	READ,
	WRITE,//一般不应该watch，除非write返回EAGAIN
	ERROR,
	CLOSE
};

class IEventManager
{
public:
	using CB = class U {
		public:
			using NR = function<void(int)>;
			using R = function<void(int, string)>;
			NR _nr;
			R _r;
			U() {}
			U(NR nr) : _nr(nr) {}
			U(R r) : _r(r) {}
			void operator()(int fd) { _nr(fd); }
			void operator()(int fd, string message) { _r(fd, message); }
			bool want_message() { return _r ? true : false; }
	};
	using EventCB = map<EventType, CB>;

	virtual bool watch(int fd, EventType event, CB callback) = 0;
	virtual bool watch(int fd, EventCB& callbacks) = 0;
	virtual bool watch(int fd, EventCB&& callbacks) = 0;
	virtual bool remove(int fd) = 0;
	/***主动close
		如fd已加入watch，则应该调用该方法关闭，否则callback无效
	****/
	virtual bool close(int fd, bool force_close) = 0;
	virtual void start() = 0;
	virtual size_t count() = 0;
	virtual void set_nonblock(int fd) = 0;
};

class EventManager : public IEventManager
{
public:
	EventManager();
	virtual bool watch(int fd, EventType event, CB callback) override;
	virtual bool watch(int fd, EventCB& callbacks) override;
	virtual bool watch(int fd, EventCB&& callbacks) override;
	virtual bool remove(int fd) override;
	virtual bool close(int fd, bool force_close = false) override;
	virtual void start() override;
	virtual size_t count() override;
	virtual void set_nonblock(int fd) override;


private:
	bool _epoll_update(int fd, int epoll_op);
	void _add_close_fd(int fd);

	int _epoll_fd;
	vector<int> _close_fds;
	map<int, EventCB> _fds;
};

