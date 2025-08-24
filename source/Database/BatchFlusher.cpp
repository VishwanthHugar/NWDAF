#include "BatchFlusher.h"
#include <iostream>

BatchFlusher::BatchFlusher(IEventRepository& repo, Config cfg)
    : repo_(repo), cfg_(cfg), running_(true), pool_(cfg.flushThreads),
    worker_(&BatchFlusher::run, this) {
}

BatchFlusher::~BatchFlusher() {
    stop();
}

void BatchFlusher::enqueue(std::shared_ptr<AnalyticsEvent> evt) {
    queue_.push(evt);
}

void BatchFlusher::stop() {
    bool expected = true;
    if (running_.compare_exchange_strong(expected, false)) {
        queue_.close();
        if (worker_.joinable()) worker_.join();
        flushAll();
        pool_.shutdown();
    }
}

void BatchFlusher::run() {
    std::vector<std::shared_ptr<AnalyticsEvent>> batch;
    auto nextFlush = std::chrono::steady_clock::now() + cfg_.flushInterval;

    while (running_ || !queue_.empty()) {
        auto eventOpt = queue_.pop();
        if (!eventOpt.has_value()) 
            continue;

        batch.push_back(eventOpt.value());

        if (batch.size() >= cfg_.maxBatchSize ||
            std::chrono::steady_clock::now() >= nextFlush) {
            auto batchCopy = batch;
            batch.clear();
            nextFlush = std::chrono::steady_clock::now() + cfg_.flushInterval;

            pool_.enqueue([this, batchCopy]() {
                repo_.insertBatch(batchCopy);
                });
        }
    }
}

void BatchFlusher::flushAll() {
    std::vector<std::shared_ptr<AnalyticsEvent>> batch;
    while (!queue_.empty()) {
        auto evt = queue_.pop();
        if (evt.has_value()) batch.push_back(evt.value());
    }

    if (!batch.empty()) {
        pool_.enqueue([this, batch]() { repo_.insertBatch(batch); });
    }
}
