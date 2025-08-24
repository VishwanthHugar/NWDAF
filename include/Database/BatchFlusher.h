#ifndef BATCH_FLUSHER_H
#define BATCH_FLUSHER_H

#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>
#include <memory>
#include "IEventRepository.h"
#include "../collection/AnalyticsEvent.h"
#include "../ThreadPool.h"
#include "../SafeQueue.h"

class BatchFlusher {
public:
    struct Config {
        std::chrono::milliseconds flushInterval;
        std::size_t maxBatchSize;
        size_t flushThreads;

        Config()
            : flushInterval(std::chrono::minutes(5)),
            maxBatchSize(500),
            flushThreads(2) {
        }
    };


    explicit BatchFlusher(IEventRepository& repo, Config cfg = Config());
    ~BatchFlusher();

    void enqueue(std::shared_ptr<AnalyticsEvent> evt);
    void stop();

private:
    void run();
    void flushAll();

    IEventRepository& repo_;
    Config cfg_;
    SafeQueue<std::shared_ptr<AnalyticsEvent>> queue_;
    std::atomic<bool> running_;
    ThreadPool pool_;
    std::thread worker_;
};

#endif // BATCH_FLUSHER_H
