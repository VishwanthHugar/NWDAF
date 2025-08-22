#include "../header/NWDAF.h"
#include "../header/3rdparty/httplib.h"
#include <iostream>


bool SubscriptionManager::addSubscription(const std::string& id, const SubscriptionInfo& info) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (subscriptions_.find(id) != subscriptions_.end())
        return false;

    subscriptions_[id] = info;
    return true;
}

bool SubscriptionManager::removeSubscription(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_);
    return subscriptions_.erase(id) > 0;
}

size_t SubscriptionManager::count() {
    std::lock_guard<std::mutex> lock(mutex_);
    return subscriptions_.size();
}

std::map<std::string, SubscriptionInfo> SubscriptionManager::getAllSubscriptions() {
    std::lock_guard<std::mutex> lock(mutex_);
    return subscriptions_;
}

std::optional<SubscriptionInfo> SubscriptionManager::getSubscription(const std::string& subId) const {
    auto it = subscriptions_.find(subId);
    if (it != subscriptions_.end()) {
        return it->second;
    }
    return std::nullopt; // subscription not found
}
