#ifndef _INCLUDE_NWDAF_COLLECTOR_H
#define _INCLUDE_NWDAF_COLLECTOR_H

#include "collection/INetworkFunction.h"
#include "SafeQueue.h"
#include "collection/AnalyticsEvent.h"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>

class NWDAFCollector {
private:
    std::vector<std::shared_ptr<INetworkFunction>> mCollectors;
    SafeQueue<std::shared_ptr<AnalyticsEvent>>& mAnalyticEventQueue;
    std::vector<std::thread> nwdafCollectorthreads;
    std::atomic<bool> mRunning{ false };

public:
    NWDAFCollector(SafeQueue<std::shared_ptr<AnalyticsEvent>>& queue);

    void addCollector(std::shared_ptr<INetworkFunction> nf);
    void startAll();
    void stopAll();
};

#endif //_INCLUDE_NWDAF_COLLECTOR_H
