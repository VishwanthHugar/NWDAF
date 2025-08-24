#ifndef _INCLUDE_NETWORK_FUNCTION_H
#define _INCLUDE_NETWORK_FUNCTION_H

#include "AnalyticsEvent.h"
#include <memory>

class INetworkFunction {
public:
    virtual ~INetworkFunction() = default;

    // Interface to collect NF data, returns a shared pointer to AnalyticsEvent
    virtual std::shared_ptr<AnalyticsEvent> collectData() = 0;
};

#endif //_INCLUDE_NETWORK_FUNCTION_H
