#pragma once
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

class IThreadPool
{
public:

    using Task = function<void(void)>;
    virtual void execute(Task t) = 0;
};


class ThreadPool : public IThreadPool
{
public:
    ThreadPool(int n = 0);

    virtual void execute(Task t) override;

private:
    void run();
    Task get_task();

    vector<thread> _threads;
    queue<Task> _q;
    mutex _mutex;
    condition_variable _cond;
};
