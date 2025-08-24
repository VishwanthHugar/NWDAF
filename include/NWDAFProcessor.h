#ifndef _INCLUDE_NWDAF_PROCESSOR_H
#define _INCLUDE_NWDAF_PROCESSOR_H

#include "Database/IEventRepository.h"
#include "collection/AnalyticsEvent.h"
#include "SafeQueue.h"
#include <thread>
#include <atomic>
#include <vector>
#include <memory>
#include <chrono>
#include <iostream>

class NWDAFProcessor {
private:
    SafeQueue<std::shared_ptr<AnalyticsEvent>>& mQueue;
    std::shared_ptr<IEventRepository> repository;
    std::thread workerThread;
    std::atomic<bool> mRunning;
    size_t batchSize;
    std::chrono::milliseconds flushInterval;

    void processLoop();

public:
    NWDAFProcessor(SafeQueue<std::shared_ptr<AnalyticsEvent>>& queue,
        std::shared_ptr<IEventRepository> repo,
        size_t batch_size = 20,
        std::chrono::milliseconds flush_ms = std::chrono::milliseconds(5000));

    ~NWDAFProcessor();

    void start();
    void stop();
};

#endif //_INCLUDE_NWDAF_PROCESSOR_H
