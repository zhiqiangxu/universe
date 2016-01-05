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
	virtual void start() = 0;
	virtual size_t count() = 0;
};

class EventManager : public IEventManager
{
public:
	EventManager();
	virtual bool watch(int fd, EventType event, CB callback) override;
	virtual bool watch(int fd, EventCB& callbacks) override;
	virtual bool watch(int fd, EventCB&& callbacks) override;
	virtual bool remove(int fd) override;
	virtual void start() override;
	virtual size_t count() override;


private:
	bool _epoll_update(int fd, int epoll_op);
	void _set_nonblock(int fd);

	int _epoll_fd;
	map<int, EventCB> _fds;
};

