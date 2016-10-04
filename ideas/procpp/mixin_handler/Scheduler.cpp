#include "Scheduler.h"
#include "ReactHandler.h"


int Scheduler::schedule_once(CB cb, float sec)
{

    struct timespec ts;
    Utils::to_timespec(sec, ts);

    return schedule_once(cb, ts);
}

int Scheduler::schedule_once(CB cb, const struct timespec ts)
{
    struct itimerspec its;
    Utils::bzero(its);
    
    Utils::memcpy(its.it_value, ts);


    return schedule(cb, its);

}

int Scheduler::schedule(CB cb, float sec, float interval)
{
    struct itimerspec its;
    Utils::bzero(its);
    
    Utils::to_timespec(sec, its.it_value);
    Utils::to_timespec(interval, its.it_interval);

    return schedule(cb, its);
}

int Scheduler::schedule(CB cb, struct itimerspec its)
{
    int timer_fd;

    if (its.it_interval.tv_sec == 0 && its.it_interval.tv_nsec == 0) {
        //once
        if (its.it_value.tv_sec == 0 && its.it_value.tv_nsec == 0) L.error_exit("You probably want to call defer instead!");

        timer_fd = _create_timerfd(its);
        if (timer_fd < 0) return timer_fd;

        watch(timer_fd, EventManager::EventCB{
            {
                EventType::READ, EventManager::CB([this, cb] (int timer_fd) {
                    uint64_t count;
                    this->read(timer_fd, &count, sizeof(count));

                    cb();

                    close(timer_fd);
                    
                })
            }
        });

    } else {
        //interval
        if (its.it_value.tv_sec == 0 && its.it_value.tv_nsec == 0) its.it_value.tv_nsec = 1;

        timer_fd = _create_timerfd(its);
        if (timer_fd < 0) return timer_fd;

        watch(timer_fd, EventManager::EventCB{
            {
                EventType::READ, EventManager::CB([this, cb] (int timer_fd) {
                    uint64_t count;
                    this->read(timer_fd, &count, sizeof(count));

                    cb();
                    
                })
            }
        });

    }

    return timer_fd;
}

int Scheduler::_create_timerfd(const struct itimerspec its)
{
    auto timer_fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK);
    if (timer_fd < 0) {
        L.error_log("timerfd_create");
        return -1;
    }

    if (timerfd_settime(timer_fd, 0/*relative*/, &its, nullptr) < 0) {
        L.error_log("timer_settime");
        return -1;
    }

    return timer_fd;
}

void Scheduler::unschedule(int timer_fd)
{
    close(timer_fd);
}

