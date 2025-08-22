#ifndef _INCLUDE_ANALYTICS_INFO_SERVICE_H
#define _INCLUDE_ANALYTICS_INFO_SERVICE_H

#include "IAnalyticsService.h"
#include "IObserver.h"
#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <memory>

class AnalyticsInfoService : public IAnalyticsService, public IObserver{
private:
    std::map<std::string, std::vector<std::function<void(const KPIData&)>>> subscriptions_;

public:
    AnalyticsInfoService() = default;

    AnalyticsResponse requestAnalytics(const AnalyticsRequest& req) override;
    void subscribeAnalytics(const std::string& nfId,
        const std::function<void(const KPIData&)>& callback) override;
    void update(const KPIData& data) override;
};

#endif //_INCLUDE_ANALYTICS_INFO_SERVICE_H
