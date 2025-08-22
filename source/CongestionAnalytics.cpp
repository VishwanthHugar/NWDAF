#include "../header/IAnalytics.h"
#include <iostream>

class CongestionAnalytics : public IAnalytics {
public:
    void analyze(const KPIData& data) override {
        if (data.value > 80.0) {
            std::cout << "[Congestion Alert] NF: " << data.nfId
                << " KPI: " << data.kpiName
                << " Value: " << data.value << std::endl;
        }
    }
};
