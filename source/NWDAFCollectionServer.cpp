#include <memory>
#include <thread>
#include <chrono>
#include <iostream>
#include "Logger.h"
#include "NWDAFCollectionServer.h"

NWDAFCollectionServer::NWDAFCollectionServer()
    : mRepository(std::make_shared<PostgresEventRepository>("dbname=telemetry user=postgres host=127.0.0.1 port=5432")), 
    /*"dbname=telemetry user=postgres password=secret host=127.0.0.1 port=5432"*/
    mNWDAFCollector(mQueue),
    mProcessor(mQueue, mRepository)
{
    // Add NF collectors
    mNWDAFCollector.addCollector(std::make_shared<AMFCollector>(mRepository));
    mNWDAFCollector.addCollector(std::make_shared<SMFCollector>(mRepository));
    mNWDAFCollector.addCollector(std::make_shared<OAMCollector>(mRepository));
    mNWDAFCollector.addCollector(std::make_shared<SliceCollector>(mRepository));
}

void NWDAFCollectionServer::start() {
    LOG_SCOPE();
    // Start data processor
    mProcessor.start();

    // Start NF collectors
    mNWDAFCollector.startAll();

}

void NWDAFCollectionServer::stop() {
    LOG_SCOPE();

    // Stop NF collectors and processor
    mNWDAFCollector.stopAll();
    mProcessor.stop();

}
