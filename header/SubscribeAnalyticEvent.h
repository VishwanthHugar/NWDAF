#ifndef _INCLUDE_SUBSCRIPTION_H
#define _INCLUDE_SUBSCRIPTION_H

#include "AnalyticsEvent.h"

// Subscription event inherits AnalyticsEventBase
struct Subscription : public AnalyticsEventBase {
    std::string callerNF;   // e.g., PCF, NSSF
    std::string snssai;     // Slice ID
    bool anySlice;

    Subscription(const std::string& subId,
        const std::string& nf,
        const std::string& event,
        const std::string& slice = "",
        bool any = false)
        : AnalyticsEventBase(event, subId),
        callerNF(nf),
        snssai(slice),
        anySlice(any) {
    }

    ~Subscription() override = default;
};

#endif //_INCLUDE_SUBSCRIPTION_H
