#include "ThreadPoolManager.h"
#include "Logger.h"

ThreadPoolManager::ThreadPoolManager(size_t minT, size_t maxT, ThreadPool& wp)
    : minThreads(minT), maxThreads(maxT), workerPool(wp), running(false) {
}

ThreadPoolManager::~ThreadPoolManager() { shutdown(); }

void ThreadPoolManager::start() {
    running = true;
    for (size_t i = 0; i < minThreads; ++i)
        dispatcherThreads.emplace_back(&ThreadPoolManager::dispatcherLoop, this);
}

void ThreadPoolManager::submit(std::function<void(ThreadPool&)> task, int priority) {
    queue.push({ priority, task });
}

void ThreadPoolManager::shutdown() {
    running = false;
    queue.close();
    for (auto& t : dispatcherThreads)
        if (t.joinable()) t.join();
}

void ThreadPoolManager::dispatcherLoop() {
    while (running) {
        auto taskOpt = queue.pop();
        if (!taskOpt) break;
        auto t = *taskOpt;
        workerPool.enqueue([f = t.func, &wp = workerPool]() { f(wp); });
    }
}
