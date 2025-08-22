#include "../header/NWDAF.h"
#include <iostream>
#include <thread>
#include <chrono>

//#include "../header/NWDAFCollector.h"
//#include "../header/DataProcessor.h"
//#include "../header/collection/Collectors.h"
#include "../header/NWDAFCollectionServer.h"

// ---------------- Main ----------------
int main() {

    NWDAFCollectionServer collectionServer;
    collectionServer.start();

    // Run simulation for 10 seconds
    //std::this_thread::sleep_for(std::chrono::seconds(10));

    NWDAFServer server(8080);
    server.run();

    //SafeQueue<std::shared_ptr<AnalyticsEvent>> queue;
    //NWDAFCollector nwdaF(queue);

    //// Add NF collectors
    //nwdaF.addCollector(std::make_shared<AMFCollector>());
    //nwdaF.addCollector(std::make_shared<SMFCollector>());
    //nwdaF.addCollector(std::make_shared<OAMCollector>());
    //nwdaF.addCollector(std::make_shared<SliceCollector>());

    //// Start processor thread
    //DataProcessor processor(queue);
    //processor.start();

    //// Start NF collectors
    //nwdaF.startAll();

    //// Run simulation for 10 seconds
    //std::this_thread::sleep_for(std::chrono::seconds(10));

    //// Stop all threads
    //nwdaF.stopAll();
    //processor.stop();

    return 0;
}
