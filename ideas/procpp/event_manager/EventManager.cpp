#include "EventManager.h"
#include <thread>
#include <sys/epoll.h>
#include <exception>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include "Protocol.h"

static string RED(string s)
{
	return "\033[1;31m" + s + "\033[0m";
}

static void error_exit(const char *s)
{
	perror(s);
	exit(1);
}

EventManager::EventManager()
{
	_epoll_fd = epoll_create(1024);
	if (_epoll_fd == -1) {
		perror("epoll_create");
		throw runtime_error("epoll_create");
	}
}

bool EventManager::watch(int fd, EventType event, EventManager::CB callback)
{
	auto ecb_iter = _fds.find(fd);
	auto added = ecb_iter != _fds.end();

	if (!added) {
		_set_nonblock(fd);
	}

	auto& ecb = added ? ecb_iter->second : _fds[fd];
	auto event_added = added ? ecb.find(event) != ecb.end() : false;
	if (!event_added) {
		ecb[event] = callback;
	}
	
	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::watch(int fd, EventManager::EventCB& callbacks)
{
	auto added = _fds.find(fd) != _fds.end();

	if (!added) {
		_set_nonblock(fd);
	} else {
		//red bold
		cout << RED("fd already added " +  to_string(fd)) << endl;
	}

	_fds[fd] = callbacks;

	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::watch(int fd, EventManager::EventCB&& callbacks)
{
	auto added = _fds.find(fd) != _fds.end();

	if (!added) {
		_set_nonblock(fd);
	}

	_fds[fd] = callbacks;

	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::remove(int fd)
{
	if (_fds.find(fd) == _fds.end()) return false;//已移出

	_fds.erase(fd);
	if (!_epoll_update(fd, EPOLL_CTL_DEL)) error_exit(string("_epoll_update " + to_string(fd)).c_str());//valid according to http://stackoverflow.com/a/584835

	return true;
}

bool EventManager::close(int fd, bool force_close)
{
	auto ret = true;

	if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::CLOSE) != _fds[fd].end()) {
		auto f = _fds[fd][EventType::CLOSE];
		if ((ret &= remove(fd)) || force_close) _add_close_fd(fd);

		f(fd);

		return ret;
	}

	if ((ret &= remove(fd)) || force_close) _add_close_fd(fd);

	return false;
}

void EventManager::start()
{
	const int kMaxEvents = 32;
	struct epoll_event events[kMaxEvents];
	while (true) {
		int timeout = -1;
		//cout << "epoll_wait" << endl;
		auto ret = epoll_wait(_epoll_fd, events, kMaxEvents, timeout);
		//cout << "ret " << ret << endl;

		if (ret == -1) {
			if (errno != EINTR) {
				//error handle
				error_exit("epoll_wait");
			}
			continue;
		}

		for (int i = 0; i < ret; i++) {
			auto fd = events[i].data.fd;
			auto flags = events[i].events;
			if (flags & EPOLLIN) {
				if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::READ) != _fds[fd].end()) {
					auto f = _fds[fd][EventType::READ];
					if (f.want_message()) {
						f(fd, Protocol::read(fd));
					} else {
						f(fd);
					}
				} else {
					//error handle
				}
			}
			if (flags & EPOLLOUT) {
				if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::WRITE) != _fds[fd].end()) {
					auto f = _fds[fd][EventType::WRITE];
					f(fd);
				} else {
					//error handle
				}
			}
			if (flags & EPOLLERR) {
				if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::ERROR) != _fds[fd].end()) {
					auto f = _fds[fd][EventType::ERROR];
					f(fd);
				} else {
					//error handle
				}
			}
			if (flags & (EPOLLRDHUP | EPOLLHUP)) {
				if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::CLOSE) != _fds[fd].end()) {
					auto f/*f为拷贝*/ = _fds[fd][EventType::CLOSE];
					if (remove(fd)) _add_close_fd(fd);
					f(fd);
				} else {
					if (remove(fd)) _add_close_fd(fd);
				}
			}

		}

		for (auto fd : _close_fds) {
			::close(fd);
		}
		_close_fds.clear();
	}
}

size_t EventManager::count()
{
	return _fds.size();
}

bool EventManager::_epoll_update(int fd, int epoll_op)
{
	struct epoll_event ev;
	ev.data.fd = fd;

	uint32_t events = EPOLLET;

	if (epoll_op != EPOLL_CTL_DEL) {
		for (const auto& r : _fds[fd]) {
			switch (r.first) {
				case EventType::READ:
					events |= EPOLLIN;
					break;
				case EventType::WRITE:
					events |= EPOLLOUT;
					break;
				case EventType::ERROR:
					break;
				case EventType::CLOSE:
					events |= EPOLLRDHUP | EPOLLHUP;
					break;
				default:
					//error handle
					break;
			}
		}
	}
	ev.events = events;

	return epoll_ctl(_epoll_fd, epoll_op, fd, &ev) != -1;
}

void EventManager::_set_nonblock(int fd)
{
	auto old_flags = fcntl(fd, F_GETFL);
	auto new_flags = old_flags | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_flags);
}

void EventManager::_add_close_fd(int fd)
{
	_close_fds.push_back(fd);
}
