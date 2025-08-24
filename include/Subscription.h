#ifndef _INCLUDE_SUBSCRIPTION_H
#define _INCLUDE_SUBSCRIPTION_H

#include "SafeQueue.h"
#include "KPIData.h"
#include "IAnalytics.h"
#include "ISubject.h"
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <algorithm>

#include "3rdparty/json.hpp"
#include <unordered_map>
#include <string>
#include <optional>

using json = nlohmann::json;

// Store subscriber information
struct SubscriptionInfo {
    std::string subscriptionId;
    std::string notifUri;   // consumer-provided notification URI
    std::string eventId;
    json analyticsFilter;
};

class SubscriptionManager {
private:
    std::map<std::string, SubscriptionInfo> subscriptions_;
    std::mutex mutex_;

public:
    bool addSubscription(const std::string& id, const SubscriptionInfo& info);
    bool removeSubscription(const std::string& id);
    size_t count();
    std::map<std::string, SubscriptionInfo> getAllSubscriptions();
    std::optional<SubscriptionInfo> getSubscription(const std::string& subId) const;

};

#endif //_INCLUDE_SUBSCRIPTION_H