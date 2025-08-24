#ifndef THREAD_POOL_MANAGER_H
#define THREAD_POOL_MANAGER_H

#include "ThreadPool.h"
#include "SafeQueue.h"
#include <vector>
#include <thread>
#include <atomic>
#include <functional>

// Task structure for dispatcher
struct DispatchTask {
    int priority;
    std::function<void(ThreadPool&)> func;
};

class ThreadPoolManager {
public:
    explicit ThreadPoolManager(size_t minThreads, size_t maxThreads, ThreadPool& workerPool);
    ~ThreadPoolManager();

    void submit(std::function<void(ThreadPool&)> task, int priority = 0);

    void start();
    void shutdown();

private:
    void dispatcherLoop();

    SafeQueue<DispatchTask> queue;
    std::vector<std::thread> dispatcherThreads;
    std::atomic<bool> running;
    size_t minThreads;
    size_t maxThreads;

    ThreadPool& workerPool; // underlying worker layer
};

#endif // THREAD_POOL_MANAGER_H
