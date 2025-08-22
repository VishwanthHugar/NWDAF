#include "../header/NWDAFCollector.h"
#include <chrono>

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
                mAnalyticEventQueue.push(event);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
