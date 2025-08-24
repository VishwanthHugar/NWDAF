#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "SafeQueue.h"
#include <vector>
#include <thread>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(std::function<void()> task);
    void shutdown();

private:
    void workerLoop();

    SafeQueue<std::function<void()>> taskQueue;
    std::vector<std::thread> workers;
    std::atomic<bool> running;
};

#endif // THREAD_POOL_H
