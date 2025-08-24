#ifndef _INCLUDE_NWDAF_COLLECTION_SERVER_H
#define _INCLUDE_NWDAF_COLLECTION_SERVER_H

#include "NWDAFCollector.h"
#include "NWDAFProcessor.h"
#include "SafeQueue.h"
#include "AnalyticsEvent.h"
#include "PostgresEventRepository.h"
#include "Collectors.h"

class NWDAFCollectionServer {
private:
    SafeQueue<std::shared_ptr<AnalyticsEvent>> mQueue;
    std::shared_ptr<IEventRepository> mRepository;
    NWDAFCollector mNWDAFCollector;
    NWDAFProcessor mProcessor;

public:
    NWDAFCollectionServer();
    void start();
    void stop();
};

#endif //_INCLUDE_NWDAF_COLLECTION_SERVER_H
