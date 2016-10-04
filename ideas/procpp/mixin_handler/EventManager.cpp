#include "EventManager.h"
#include <thread>
#include <sys/epoll.h>
#include <exception>
#include <errno.h>
#include <unistd.h>//read
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

    //use a different epoll fd after fork
    this->on<EventManager::ON_FORK>(Utils::to_function([this]() {
        ::close(_epoll_fd);

        _epoll_fd = epoll_create(1024);
        if (_epoll_fd == -1) {
            perror("epoll_create");
            throw runtime_error("epoll_create");
        }

        for (const auto& r : _fds) {
            if (!_epoll_update(r.first, EPOLL_CTL_ADD)) throw runtime_error("_epoll_update");
        }
    }));
}

EventManager::EventCB EventManager::to_ecb(EventManager::CB::C connect_callback)
{
    return EventCB{
            {
                EventType::CONNECT, CB(connect_callback)
            }
        };
}

bool EventManager::watch(int fd, EventType event, EventManager::CB callback, bool et)
{
    auto ecb_iter = _fds.find(fd);
    auto added = ecb_iter != _fds.end();

    // TODO 当所有socket都通过nonblock_socket/set_nonblock时，即可干掉这行
    if (!added) {
        Utils::set_nonblock(fd);
    }

    auto& ecb = added ? ecb_iter->second : _fds[fd];
    ecb[event] = callback;
    
    if (fd == _current_fd) {
        _current_cb[event] = callback;
    }

    return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD, et);
}

bool EventManager::watch(int fd, EventManager::EventCB&& callbacks, bool re_watch, bool et)
{
    auto added = _fds.find(fd) != _fds.end();

    //re_watch表示该fd并不在epoll set，所以不可能是_current_fd
    if (re_watch) {
        added = false;
    } else {

        if (fd == _current_fd) {
            _current_cb = callbacks;
        }

    }

    if (!added) {
        Utils::set_nonblock(fd);
    }

    _fds[fd] = move(callbacks);//move is needed for local rvalue reference

    return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD, et);
}

bool EventManager::auto_rewatch(int fd, bool et)
{

    auto ecb_iter = _fds.find(fd);
    auto added = ecb_iter != _fds.end();

    // TODO 当所有socket都通过nonblock_socket/set_nonblock时，即可干掉这行
    if (!added) {
        Utils::set_nonblock(fd);
    }

    return _epoll_update(fd, added ? EPOLL_CTL_MOD : EPOLL_CTL_ADD, et);

}


bool EventManager::unwatch(int fd, bool no_callback)
{
    if (_fds.find(fd) == _fds.end()) return false;//已移出

    auto cb = _fds[fd];//拷贝

    if (fd == _current_fd) {
        _current_cb.clear();
    }

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

//TODO 需要等_write_buf发完再关闭
bool EventManager::close(int fd, bool force_close)
{
    auto ret = true;

    if (_fd_ssl.find(fd) != _fd_ssl.end()) {
        auto p_ssl_conn = _fd_ssl[fd];
        delete p_ssl_conn;
        _fd_ssl.erase(fd);
    }
    _write_buf.erase(fd);

    if (_fds.find(fd) != _fds.end() && _fds[fd].find(EventType::CLOSE) != _fds[fd].end()) {
        auto f = _fds[fd][EventType::CLOSE];
        if ((ret &= unwatch(fd)) || force_close) _add_close_fd(fd);

        f(fd);//TODO make it possible to transfer state and buffer when close

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

bool EventManager::_has_write_buf(int fd)
{
    return _write_buf.find(fd) != _write_buf.end();
}

void EventManager::_append_write_buf(int fd, const void *buf, size_t count)
{
    auto it = _write_buf.find(fd);
    if (it == _write_buf.end()) {
        _write_buf[fd] = string((const char*)buf, count);
        auto_rewatch(fd);
    }
    else it->second.append((const char*)buf, count);
}

ssize_t EventManager::write(int fd, const void *buf, size_t count, int* p_errno)
{
    if (_fd_ssl.find(fd) != _fd_ssl.end()) return _fd_ssl[fd]->write(buf, count);

    if (_has_write_buf(fd)) {
        _append_write_buf(fd, buf, count);
        return 0;
    }

    size_t size = 0;

    do {
        auto ret = ::write(fd, (char*)buf + size, count - size);
        if (ret == -1) {

            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                _append_write_buf(fd, (const char*)buf + size, count - size);
                return size;
            }

            L.error_log("write fail size = " + to_string(size));

            if (errno == EPIPE) close(fd);

            if (p_errno) *p_errno = errno;

            return -1;
        }

        size += ret;
    } while ( size < count );

    return count;
}

ssize_t EventManager::write(int fd, const string& data, int* p_errno)
{
    return write(fd, data.data(), data.length(), p_errno);
}

ssize_t EventManager::write_line(int fd, const string message)
{
    auto l = message + "\n";
    return write(fd, l.data(), l.length());
}

ssize_t EventManager::sendto(int u_sock, const void *buf, size_t len, int flags, const struct sockaddr* dest_addr, socklen_t addrlen)
{
    return ::sendto(u_sock, buf, len, flags, dest_addr, addrlen);
}

string EventManager::read(int fd)
{

    if (_fd_ssl.find(fd) == _fd_ssl.end()) {

        string message;
        char buf[1024];

        while (true) {
            auto size = ::read(fd, buf, sizeof(buf));
            if (size == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    return message;
                } else {
                    //error handle
                }
                continue;
            } else if (size == 0) {
                //eof
                break;
            }

            // TODO 避免拷贝
            message.append(buf, size);
        }

        return message;

    } else {
        return _fd_ssl[fd]->read();
    }

}

ssize_t EventManager::read(int fd, void* buf, size_t size)
{
    if (_fd_ssl.find(fd) == _fd_ssl.end()) return ::read(fd, buf, size);
    else return _fd_ssl[fd]->read(buf, size);
}

string EventManager::read_until(int fd, string separator)
{
    L.error_exit("no read_until");
    return "no impl";
}

static bool S_exit = false;
static void _at_exit(int sig)
{
    S_exit = true;
}

void EventManager::event_loop()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, _at_exit);

    // do global initialization here
    if (!SSLConnection::initialize_ssl()) L.error_exit("initialize_ssl failed");

    const int kMaxEvents = 32;
    struct epoll_event events[kMaxEvents];
    while (true) {
        if (S_exit) return;

        int timeout = -1;
        //cout << "epoll_wait" << endl;
        auto ret = epoll_wait(_epoll_fd, events, kMaxEvents, timeout);
        //cout << "ret " << ret << endl;

        handle_events(ret, events);
    }
}

void EventManager::handle_events(int ret, const struct epoll_event* events)
{
    if (ret == -1) {
        if (errno != EINTR) {
            //error handle
            L.error_exit("epoll_wait");
        }
        return;
    }

    for (int i = 0; i < ret; i++) {
        _current_fd = events[i].data.fd;
        auto has_callback = _fds.find(_current_fd) != _fds.end();
        if (!has_callback) {
            //可能是某个回调unwatch或者close了该fd
            cout << Utils::RED("waited fd has no callback " + to_string(_current_fd) + " pid = " + to_string(Utils::getpid())) << endl;
            continue;
        }

        auto flags = events[i].events;

        _current_cb = _fds[_current_fd];
        if (_current_cb.find(EventType::CONNECT) != _current_cb.end()) {
            auto f/*copy to avoid erase when calling*/ = _current_cb[EventType::CONNECT];
            if (flags & (EPOLLERR|EPOLLHUP)) {
                goto CONNECT_NG;
            }

            if (flags & EPOLLOUT) {
                int result;
                socklen_t result_len = sizeof(result);
                if (getsockopt(_current_fd, SOL_SOCKET, SO_ERROR, &result, &result_len) < 0) {
                    goto CONNECT_NG;
                }

                if (result != 0) {
                    goto CONNECT_NG;
                } else {
                    goto CONNECT_OK;
                }
            }

CONNECT_NG:

            /*******实现要求*******
             **********************
             **** 1.只回调一次 ****
            ***** 2.fd占住(所以不能close) ********
            ***********************
            **/
            _fds[_current_fd].erase(EventType::CONNECT);//so GAME_OVER is not fired
            _current_cb.erase(EventType::CONNECT);
            unwatch(_current_fd);
            f(_current_fd, ConnectResult::NG);
            continue;

CONNECT_OK:
            if (_fd_ssl.find(_current_fd) != _fd_ssl.end()) _fd_ssl[_current_fd]->handshake();
            //回调前就删除
            _fds[_current_fd].erase(EventType::CONNECT);
            _current_cb.erase(EventType::CONNECT);
            f(_current_fd, ConnectResult::OK);
        }

        if (flags & EPOLLIN) {
            if (_fd_ssl.find(_current_fd) == _fd_ssl.end() || _fd_ssl[_current_fd]->handshake()) {

                if (_current_cb.find(EventType::READ) != _current_cb.end()) {
                    auto f/*copy to avoid erase when calling*/ = _current_cb[EventType::READ];
                    if (f.want_message()) {
                        f(_current_fd, read(_current_fd));
                    } else {
                        f(_current_fd);
                    }
                } else {
                    L.error_log("fd " + to_string(_current_fd) + " has no read handler");
                    //error handle
                }

            }
        }
        if (flags & EPOLLOUT) {
            bool write_buf_done = _finish_write_buf(_current_fd);

            if (write_buf_done) {

                auto has_write_cb = _current_cb.find(EventType::WRITE) != _current_cb.end();
                if (has_write_cb) {
                    auto f = _current_cb[EventType::WRITE];
                    f(_current_fd);
                } else {
                    auto_rewatch(_current_fd);
                }

            }
        }

        if (flags & EPOLLERR) {
            if (_current_cb.find(EventType::ERROR) != _current_cb.end()) {

                auto f = _current_cb[EventType::ERROR];
                f(_current_fd);

            } else {
                // if no error handler,force trigger close handler
                flags |= EPOLLHUP;
            }
        }

        if (flags & (EPOLLRDHUP | EPOLLHUP)) {

            // unwatch may fail if it's already closed in READ handler

            L.debug_log("closed by peer : " + to_string(_current_fd));

            if (_current_cb.find(EventType::CLOSE) != _current_cb.end()) {

                auto f/*copy to avoid erase when calling*/ = _current_cb[EventType::CLOSE];
                if (unwatch(_current_fd)) {
                    _add_close_fd(_current_fd);
                    f(_current_fd);//TODO make it possible to transfer state and buffer when close
                }

            } else {
                //no close handler registered, try to unwatch + close
                if (unwatch(_current_fd)) _add_close_fd(_current_fd);
            }
        }

    }

    _current_fd = -1;

    for (auto fd : _close_fds) {
        ::close(fd);
    }
    _close_fds.clear();

}

size_t EventManager::count_fds()
{
    return _fds.size();
}

bool EventManager::_epoll_update(int fd, int epoll_op, bool et)
{
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;

    // 经测试，ET模式下，即使上一个IN没处理，下一个IN进来后，仍然会触发，但只会触发一次
    uint32_t events = et ? EPOLLET : 0;

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

        if (_has_write_buf(fd)) events |= EPOLLOUT;
    }
    ev.events = events;

    return epoll_ctl(_epoll_fd, epoll_op, fd, &ev) != -1;
}

void EventManager::_add_close_fd(int fd)
{
    _close_fds.push_back(fd);
}

//TODO 性能待提高
bool EventManager::_finish_write_buf(int fd)
{
    if (!_has_write_buf(fd)) return true;

    auto buf = _write_buf[fd];
    _write_buf.erase(fd);

    auto size = write(fd, buf);

    if (size < 0) return false;

    L.debug_log(string("finish_write_buf ") + ((size_t)size == buf.length() ? "ok" : "ng"));
    return (size_t)size == buf.length();

}


