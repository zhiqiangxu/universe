#include "EventManager.h"
#include <thread>
#include <sys/epoll.h>
#include <exception>
#include <errno.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fcntl.h>
#include <strings.h>//bzero
#include "Protocol.h"

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

static void error_log(const char *s)
{
	perror(s);
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

	// TODO 当所有socket都通过nonblock_socket/set_nonblock时，即可干掉这行
	if (!added) {
		set_nonblock(fd);
	}

	auto& ecb = added ? ecb_iter->second : _fds[fd];
	auto event_added = added ? ecb.find(event) != ecb.end() : false;
	if (!event_added) {
		ecb[event] = callback;
	}
	
	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::watch(int fd, const EventManager::EventCB& callbacks, bool re_watch)
{
	auto added = _fds.find(fd) != _fds.end();

	if (re_watch) added = false;

	if (!added) {
		set_nonblock(fd);
	} else {
		//red bold
		cout << RED("fd " +  to_string(fd) + " already added") << endl;
	}

	_fds[fd] = callbacks;

	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::watch(int fd, EventManager::EventCB&& callbacks, bool re_watch)
{
	if (callbacks.size() == 0) {
		cout << RED("watch without callback is invalid") << endl;
		return false;
	}

	auto added = _fds.find(fd) != _fds.end();

	if (!added) {
		set_nonblock(fd);
	}

	_fds[fd] = callbacks;

	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::remove(int fd)
{
	if (_fds.find(fd) == _fds.end()) return false;//已移出

	auto cb = _fds[fd];//拷贝

	_fds.erase(fd);
	if (!_epoll_update(fd, EPOLL_CTL_DEL)) error_exit(string("_epoll_update " + to_string(fd)).c_str());//valid according to http://stackoverflow.com/a/584835
	//must callback before erase, otherwise ML happens
	if (cb.find(EventType::CONNECT) != cb.end()) {
		cb[EventType::CONNECT](fd, ConnectResult::GAME_OVER);
	}

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
			auto has_callback = _fds.find(fd) != _fds.end();
			if (!has_callback) {
				//可能是某个回调remove或者close了该fd
				cout << RED("wait fd has no callback" + to_string(fd)) << endl;
				continue;
			}

			auto flags = events[i].events;

			auto/*TODO 拷贝性能优化*/ cb = _fds[fd];
			if (cb.find(EventType::CONNECT) != cb.end()) {
				auto f/*f为拷贝*/ = cb[EventType::CONNECT];
				if (flags & (EPOLLERR|EPOLLHUP)) {
					goto CONNECT_NG;
				}

				if (flags & EPOLLOUT) {
					int result;
					socklen_t result_len = sizeof(result);
					if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0) {
						goto CONNECT_NG;
					}

					if (result != 0) {
						goto CONNECT_NG;
					} else {
						goto CONNECT_OK;
					}
				}

CONNECT_NG:
				//下面这行会导致多次触发connect回调, 而且，connect失败fd仍需占住，
				//所以，将关闭socket的事情交给client
				//if (remove(fd)) _add_close_fd(fd);
				f(fd, ConnectResult::NG);
				continue;

CONNECT_OK:
				f(fd, ConnectResult::OK);
				//回调完就删除
				_fds[fd].erase(EventType::CONNECT);
			}

			if (flags & EPOLLIN) {
				if (cb.find(EventType::READ) != cb.end()) {
					auto f = cb[EventType::READ];
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
				if (cb.find(EventType::WRITE) != cb.end()) {

					auto f = cb[EventType::WRITE];
					f(fd);

				}
			}

			if (flags & EPOLLERR) {
				if (cb.find(EventType::ERROR) != cb.end()) {

					auto f = cb[EventType::ERROR];
					f(fd);

				} else {
					cout << RED("EPOLLERR no handler") << endl;
				}
			}

			if (flags & (EPOLLRDHUP | EPOLLHUP)) {
				if (cb.find(EventType::CLOSE) != cb.end()) {

					auto f/*f为拷贝*/ = cb[EventType::CLOSE];
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
	bzero(&ev, sizeof(ev));
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
				case EventType::CONNECT:
					events |= EPOLLOUT | EPOLLHUP;
				default:
					//error handle
					break;
			}
		}
	}
	ev.events = events;

	return epoll_ctl(_epoll_fd, epoll_op, fd, &ev) != -1;
}

void EventManager::set_nonblock(int fd)
{
	auto old_flags = fcntl(fd, F_GETFL);
	auto new_flags = old_flags | O_NONBLOCK;
	if (fcntl(fd, F_SETFL, new_flags) == -1) error_exit("set_nonblock");
}

int EventManager::nonblock_socket(int domain, int type, int protocol)
{
	auto s = socket(domain, type, protocol);
	if (s == -1) {
		error_log("socket");
		return -1;
	}

	set_nonblock(s);
	return s;
}

void EventManager::_add_close_fd(int fd)
{
	_close_fds.push_back(fd);
}
