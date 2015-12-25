#include "EventManager.h"
#include <thread>
#include <sys/epoll.h>
#include <exception>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

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
		auto old_flags = fcntl(fd, F_GETFL);
		auto new_flags = old_flags | O_NONBLOCK;
		fcntl(fd, F_SETFL, new_flags);
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
	_fds[fd] = callbacks;

	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::watch(int fd, EventManager::EventCB&& callbacks)
{
	auto added = _fds.find(fd) != _fds.end();
	_fds[fd] = callbacks;

	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::_epoll_update(int fd, int epoll_op)
{
	struct epoll_event ev;
	ev.data.fd = fd;

	uint32_t events = EPOLLET;

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
	ev.events = events;

	return epoll_ctl(_epoll_fd, epoll_op, fd, &ev) != -1;
}

bool EventManager::remove(int fd)
{
	auto suc = _epoll_update(fd, EPOLL_CTL_DEL);
	if (suc) {
		_fds.erase(fd);
	}

	return suc;
}

void EventManager::start()
{
	const int kMaxEvents = 32;
	struct epoll_event events[kMaxEvents];
	while (true) {
		int timeout = 0;
		auto ret = epoll_wait(_epoll_fd, events, kMaxEvents, timeout);
		if (ret != 0) {
			cout << "ret:" << ret << endl;
		}

		if (ret == -1) {
			if (errno != EINTR) {
				//error handle
			}
			continue;
		}

		for (int i = 0; i < ret; i++) {
			auto fd = events[i].data.fd;
			auto flags = events[i].events;
			if (flags | EPOLLIN) {
				if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::READ) != _fds[fd].end()) {
					auto f = _fds[fd][EventType::READ];
					f(fd);
				} else {
					//error handle
				}
			}
			if (flags | EPOLLOUT) {
				if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::WRITE) != _fds[fd].end()) {
					auto f = _fds[fd][EventType::WRITE];
					f(fd);
				} else {
					//error handle
				}
			}
			if (flags | EPOLLERR) {
				if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::ERROR) != _fds[fd].end()) {
					auto f = _fds[fd][EventType::ERROR];
					f(fd);
				} else {
					//error handle
				}
			}
			if (flags | (EPOLLRDHUP | EPOLLHUP)) {
				remove(fd);
				close(fd);
				if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::ERROR) != _fds[fd].end()) {
					auto f = _fds[fd][EventType::ERROR];
					f(fd);
				} else {
					//error handle
				}
			}

		}
	}
}


