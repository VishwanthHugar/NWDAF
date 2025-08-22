#ifndef _INCLUDE_NWDAF_COLLECTION_SERVER_H
#define _INCLUDE_NWDAF_COLLECTION_SERVER_H

#include "SafeQueue.h"
#include "NWDAFCollector.h"
#include "DataProcessor.h"
#include "collection/Collectors.h"
#include <memory>

class NWDAFCollectionServer {
private:
    SafeQueue<std::shared_ptr<AnalyticsEvent>> mQueue;
    NWDAFCollector mNWDAFCollector;
    DataProcessor mProcessor;

public:
    NWDAFCollectionServer();
    void start();
    void stop();
};

#endif //_INCLUDE_NWDAF_COLLECTION_SERVER_H
