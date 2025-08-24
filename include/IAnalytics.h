#ifndef _INCLUDE_I_ANALYTICS_H
#define _INCLUDE_I_ANALYTICS_H

#include "KPIData.h"

class IAnalytics {
public:
    virtual void analyze(const KPIData& data) = 0;
    virtual ~IAnalytics() {}
};

#endif //_INCLUDE_I_ANALYTICS_H
