#ifndef _INCLUDE_I_ANALYTICS_SERVICE_H
#define _INCLUDE_I_ANALYTICS_SERVICE_H

#include "KPIData.h"
#include <string>
#include <functional>

struct AnalyticsRequest {
    std::string nfId;
    std::string kpiName;
};

struct AnalyticsResponse {
    std::string nfId;
    std::string kpiName;
    double value;
    std::string status; // "OK" or "ERROR"
};

class IAnalyticsService {
public:
    virtual AnalyticsResponse requestAnalytics(const AnalyticsRequest& req) = 0;
    virtual void subscribeAnalytics(const std::string& nfId,
        const std::function<void(const KPIData&)>& callback) = 0;
    virtual ~IAnalyticsService() = default;
};

#endif //_INCLUDE_I_ANALYTICS_SERVICE_H
