#ifndef _INCLUDE_CONSOLE_OBSERVER_H
#define _INCLUDE_CONSOLE_OBSERVER_H

#include "IObserver.h"
#include "../include/Logger.h"
#include <iostream>

class ConsoleObserver : public IObserver {
public:
    void update(const KPIData& data) override {
        LOG_INFO_FILE("Received KPI: " + data.kpiName +
                 " value: " + std::to_string(data.value) +
                 " from NF: " + data.nfId);
    }
};
#endif //_INCLUDE_CONSOLE_OBSERVER_H