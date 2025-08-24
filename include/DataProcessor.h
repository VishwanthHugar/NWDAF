#ifndef _INCLUDE_DATA_PROCESSOR_H
#define _INCLUDE_DATA_PROCESSOR_H

#include "SafeQueue.h"
#include "collection/AnalyticsEvent.h"
#include <thread>
#include <atomic>
#include <iostream>

class DataProcessor {
private:
    SafeQueue<std::shared_ptr<AnalyticsEvent>>& queue_;
    std::atomic<bool> running_{ true };
    std::thread processingThread_;

public:
    DataProcessor(SafeQueue<std::shared_ptr<AnalyticsEvent>>& queue)
        : queue_(queue) {
    }

    void start() {
        processingThread_ = std::thread([this]() {
            while (running_) {
                auto eventOpt = queue_.pop();
                if (eventOpt) {
                    auto event = eventOpt.value();
                    /*std::cout << "[" << event->eventId << "] "
                        << event->subscriptionId
                        << " @ " << event->timestamp
                        << "\n";*/
                }
            }
            });
    }

    void stop() {
        running_ = false;
        if (processingThread_.joinable())
            processingThread_.join();
    }
};

#endif //_INCLUDE_DATA_PROCESSOR_H
