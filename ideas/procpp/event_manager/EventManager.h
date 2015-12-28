#pragma once
#include <functional>
#include <vector>
#include <tuple>
#include <map>
#include <string>

using namespace std;

enum class EventType
{
	READ,
	WRITE,
	ERROR,
	CLOSE
};

class IEventManager
{
public:
	using CB = function<void(int)>;
	using EventCB = map<EventType, CB>;

	virtual bool watch(int fd, EventType event, CB callback) = 0;
	virtual bool watch(int fd, EventCB& callbacks) = 0;
	virtual bool watch(int fd, EventCB&& callbacks) = 0;
	virtual bool remove(int fd) = 0;
	virtual void start() = 0;
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

private:
	bool _epoll_update(int fd, int epoll_op);
	void _set_nonblock(int fd);
	map<int, EventCB> _fds;
	int _epoll_fd;
};

