#ifndef _INCLUDE_KPI_DATA_H
#define _INCLUDE_KPI_DATA_H

#include <string>

struct KPIData {
    std::string nfId;
    std::string kpiName;
    std::string sliceId;

    double value;
    long timestamp;
};

#endif //_INCLUDE_KPI_DATA_H
