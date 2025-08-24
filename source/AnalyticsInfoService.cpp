#include "AnaticalService.h"
#include <iostream>
#include "Logger.h"

AnalyticsResponse AnalyticsInfoService::requestAnalytics(const AnalyticsRequest& req) {
    AnalyticsResponse resp;
    resp.nfId = req.nfId;
    resp.kpiName = req.kpiName;
    resp.status = "OK";
    resp.value = 50.0 + (rand() % 50);

    std::cout << "[AnalyticsInfoService] On-demand request: "
        << resp.kpiName << " = " << resp.value << std::endl;
    return resp;
}

void AnalyticsInfoService::subscribeAnalytics(const std::string& nfId,
    const std::function<void(const KPIData&)>& callback) {
    subscriptions_[nfId].push_back(callback);
    std::cout << "[AnalyticsInfoService] NF " << nfId << " subscribed for updates." << std::endl;
}

void AnalyticsInfoService::update(const KPIData& data) {
    auto it = subscriptions_.find(data.nfId);
    if (it != subscriptions_.end()) {
        for (auto& cb : it->second) {
            cb(data);
        }
    }
}
