#include "../header/NWDAFCollectionServer.h"
#include <memory>
#include <thread>
#include <chrono>
#include <iostream>

NWDAFCollectionServer::NWDAFCollectionServer()
    : mNWDAFCollector(mQueue), mProcessor(mQueue)
{
    // Add NF collectors
    mNWDAFCollector.addCollector(std::make_shared<AMFCollector>());
    mNWDAFCollector.addCollector(std::make_shared<SMFCollector>());
    mNWDAFCollector.addCollector(std::make_shared<OAMCollector>());
    mNWDAFCollector.addCollector(std::make_shared<SliceCollector>());
}

void NWDAFCollectionServer::start() {
    // Start data processor
    mProcessor.start();

    // Start NF collectors
    mNWDAFCollector.startAll();

    std::cout << "NWDAF Collection Server started.\n";
}

void NWDAFCollectionServer::stop() {
    // Stop NF collectors and processor
    mNWDAFCollector.stopAll();
    mProcessor.stop();

    std::cout << "NWDAF Collection Server stopped.\n";
}
