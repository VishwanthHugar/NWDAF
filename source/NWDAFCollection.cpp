#include "NWDAFCollector.h"
#include <chrono>
#include <iostream>
#include "Logger.h"

NWDAFCollector::NWDAFCollector(SafeQueue<std::shared_ptr<AnalyticsEvent>>& queue)
    : mAnalyticEventQueue(queue) {
}

void NWDAFCollector::addCollector(std::shared_ptr<INetworkFunction> nf) {
    mCollectors.push_back(nf);
}

void NWDAFCollector::startAll() {
    mRunning = true;
    for (auto& c : mCollectors) {
        nwdafCollectorthreads.emplace_back([this, c]() {
            while (mRunning) {
                auto event = c->collectData();
                if (event) {
                    std::cout << "[NWDAFCollector] Before push the event, mAnalyticEventQueue.size: " << mAnalyticEventQueue.size() << std::endl;
                    mAnalyticEventQueue.push(event);
                    std::cout << "[NWDAFCollector] push the event, mAnalyticEventQueue.size: "<< mAnalyticEventQueue.size() << std::endl;
                }
                else
                {
                    std::cout << "[NWDAFCollector] event empty, mAnalyticEventQueue.size: " << mAnalyticEventQueue.size() << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            }
            });
    }
}

void NWDAFCollector::stopAll() {
    mRunning = false;
    for (auto& t : nwdafCollectorthreads) {
        if (t.joinable()) t.join();
    }
    mAnalyticEventQueue.close();
}
