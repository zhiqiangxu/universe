#pragma once
#include <functional>
#include <time.h>
#include <vector>
#include <sys/timerfd.h>
#include "EventManager.h"

using namespace std;


class IScheduler
{
public:
	using CB = function<void(void)>;

	virtual int schedule_once(CB cb, float sec) = 0;

	virtual int schedule(CB cb, float sec, float interval) = 0;

	virtual int schedule_once(CB cb, const struct timespec ts) = 0;

	virtual int schedule(CB cb, struct itimerspec ts) = 0;

	virtual void unschedule(int timer_fd) = 0;
};

class Scheduler : public IScheduler, public EventManager
{
public:

	using CB = IScheduler::CB;

	virtual int schedule_once(CB cb, float sec) override;

	virtual int schedule(CB cb, float sec, float interval) override;

	virtual int schedule_once(CB cb, const struct timespec ts) override;

	virtual int schedule(CB cb, struct itimerspec its) override;

	virtual void unschedule(int timer_fd) override;

private:

	virtual int _create_timerfd(const struct itimerspec its);

	vector<CB> _update_cbs;
	vector<CB> _once_cbs;

};
