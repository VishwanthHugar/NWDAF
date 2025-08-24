#ifndef _INCLUDE_I_OBSERVER_H
#define _INCLUDE_I_OBSERVER_H

#include "KPIData.h"

class IObserver {
public:
    virtual void update(const KPIData& data) = 0;
    virtual ~IObserver() = default;
};

#endif //_INCLUDE_I_OBSERVER_H
