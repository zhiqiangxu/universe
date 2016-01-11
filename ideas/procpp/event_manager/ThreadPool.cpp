#include "ThreadPool.h"
#include <mutex>
#include <iostream>

static string RED(string s)
{
	return "\033[1;31m" + s + "\033[0m";
}

static int get_cpu_cores()
{
	auto n = thread::hardware_concurrency();

	if (n == 0) {
		cout << "hardware_concurrency returns 0" << endl;
		n = 8;
	}

	return n;
}

void run()
{
}

ThreadPool::ThreadPool(int n)
{
	if (n <= 0) n = get_cpu_cores();

	for (int i = 0; i < n; i++) {
		_threads.push_back(thread{ [this] { run(); } });
	}
}

void ThreadPool::execute(Task t)
{
	lock_guard<mutex> lock(_mutex);
	_q.push(t);

	_cond.notify_one();
}

void ThreadPool::run()
{
	while (true) {
		auto t = get_task();
		t();
	}
}

ThreadPool::Task ThreadPool::get_task()
{
	unique_lock<mutex> lock(_mutex);
	if (_q.empty()) {
		//cout << "sleeping" << endl;
		_cond.wait(lock, [this] { return !_q.empty(); });
		//cout << "woken up" << endl;
	}

	auto t = _q.front();
	_q.pop();

	return t;
}

