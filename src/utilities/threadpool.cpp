#include "utilities/threadpool.h"
#include "debugmsg.h"

using namespace std;

nemesis::ThreadPool::ThreadPool(size_t threads)
{
    workers.reserve(threads);

    for (size_t i = 0; i < threads; ++i)
    {
        newWorker();
    }
}

void nemesis::ThreadPool::join_all()
{
    if (!started) return;

    sync = true;
    condition.notify_all();

    for (auto& worker : workers)
    {
        worker.join();
    }
}

void nemesis::ThreadPool::stop()
{
    {
        lock_guard lock{queue_mutex};
        abort = true;
    }

    condition.notify_all();
}

nemesis::ThreadPool::~ThreadPool()
{
    stop();

    if (!sync) join_all();
}

void nemesis::ThreadPool::newWorker()
{
    workers.emplace_back(
        [&]
        {
            for (;;)
            {
                function<void()> task;

                {
                    unique_lock<mutex> lock(queue_mutex);
                    condition.wait(lock, [&] { return error || abort || sync || !tasks.empty(); });

                    if (error || abort || tasks.empty()) return;

                    task = tasks.front();
                    tasks.pop();
                }

                task();
            }
        });
}
