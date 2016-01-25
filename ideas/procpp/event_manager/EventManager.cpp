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
#include <signal.h>//signal
#include <unistd.h>//write
#include "ReactHandler.h"

template<>
const char* Utils::enum_strings<ConnectResult>::data[] = {"OK", "NG", "GAME_OVER"};



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
		Utils::set_nonblock(fd);
	}

	auto& ecb = added ? ecb_iter->second : _fds[fd];
	ecb[event] = callback;
	
	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::watch(int fd, const EventManager::EventCB& callbacks, bool re_watch)
{
	auto added = _fds.find(fd) != _fds.end();

	if (re_watch) added = false;

	if (!added) {
		Utils::set_nonblock(fd);
	}

	_fds[fd] = callbacks;

	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::watch(int fd, EventManager::EventCB&& callbacks, bool re_watch)
{
	if (callbacks.size() == 0) {
		cout << Utils::RED("watch without callback is invalid") << endl;
		return false;
	}

	auto added = _fds.find(fd) != _fds.end();

	if (!added) {
		Utils::set_nonblock(fd);
	}

	_fds[fd] = callbacks;

	return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD);
}

bool EventManager::unwatch(int fd, bool no_callback)
{
	if (_fds.find(fd) == _fds.end()) return false;//已移出

	auto cb = _fds[fd];//拷贝

	_fds.erase(fd);
	if (!_epoll_update(fd, EPOLL_CTL_DEL)) L.error_exit(string("_epoll_update " + to_string(fd)).c_str());//valid according to http://stackoverflow.com/a/584835

	// 调用方负责回调
	if (no_callback) return true;

	//must callback, otherwise ML happens
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
		if ((ret &= unwatch(fd)) || force_close) _add_close_fd(fd);

		f(fd);

		return ret;
	}

	if ((ret &= unwatch(fd)) || force_close) _add_close_fd(fd);

	return true;
}

bool EventManager::close_all()
{
	auto _copy = _fds;
	for (const auto& r : _copy) {
		if (!close(r.first)) L.error_exit(("clear failed for " + to_string(r.first)).c_str());
	}

	return true;
}

bool EventManager::_destroy()
{

	::close(_epoll_fd);
	for (auto fd : _close_fds) {
		::close(fd);
	}

	return true;
}

ssize_t EventManager::write(int fd, const void *buf, size_t count)
{
	auto ret = ::write(fd, buf, count);
	if (ret == -1) {
		if (errno == EPIPE) close(fd);
	}

	return ret;
}

ssize_t EventManager::write_line(int fd, const string message)
{
	auto l = message + "\n";
	return write(fd, l.data(), l.length());
}

void EventManager::start()
{
	signal(SIGPIPE, SIG_IGN);

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
				L.error_exit("epoll_wait");
			}
			continue;
		}

		for (int i = 0; i < ret; i++) {
			auto fd = events[i].data.fd;
			auto has_callback = _fds.find(fd) != _fds.end();
			if (!has_callback) {
				//可能是某个回调unwatch或者close了该fd
				cout << Utils::RED("wait fd has no callback " + to_string(fd)) << endl;
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
				//下面的写法会导致多次触发connect回调, 而且，connect失败fd仍需占住，
				//所以，将关闭socket的事情交给client
				//if (unwatch(fd)) _add_close_fd(fd);
				//f(fd, ConnectResult::NG);

				unwatch(fd);
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
					cout << Utils::RED("EPOLLERR no handler") << endl;
				}
			}

			if (flags & (EPOLLRDHUP | EPOLLHUP)) {
				if (cb.find(EventType::CLOSE) != cb.end()) {

					auto f/*f为拷贝*/ = cb[EventType::CLOSE];
					if (unwatch(fd)) _add_close_fd(fd);
					f(fd);

				} else {
					if (unwatch(fd)) _add_close_fd(fd);
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

	// 经测试，ET模式下，即使上一个IN没处理，下一个IN进来后，仍然会触发，但只会触发一次
	uint32_t events = EPOLLET;//TODO ET可配置

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

void EventManager::_add_close_fd(int fd)
{
	_close_fds.push_back(fd);
}
