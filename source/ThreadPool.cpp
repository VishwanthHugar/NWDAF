#include "ThreadPool.h"
#include "Logger.h"

ThreadPool::ThreadPool(size_t numThreads) : running(true) {
    for (size_t i = 0; i < numThreads; ++i)
        workers.emplace_back(&ThreadPool::workerLoop, this);
}

ThreadPool::~ThreadPool() { shutdown(); }

void ThreadPool::enqueue(std::function<void()> task) {
    taskQueue.push(task);
}

void ThreadPool::shutdown() {
    running = false;
    taskQueue.close();
    for (auto& t : workers)
        if (t.joinable()) t.join();
}

void ThreadPool::workerLoop() {
    while (running) {
        auto taskOpt = taskQueue.pop();
        if (!taskOpt) break;
        try { 
            (*taskOpt)(); 
        }
        catch (...) {}
    }
}
