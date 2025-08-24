#include "NWDAF.h"
#include <iostream>
#include <thread>
#include <chrono>

//#include "NWDAFCollector.h"
//#include "DataProcessor.h"
//#include "Collectors.h"
#include "NWDAFCollectionServer.h"
#include "Logger.h"
// ---------------- Main ----------------
int main() {
    LOG_SCOPE();

    LOG_INFO_FILE("Starting NWDAF Collection Server...");
    NWDAFCollectionServer collectionServer;
    collectionServer.start();

    // Run simulation for 10 seconds
    //std::this_thread::sleep_for(std::chrono::seconds(10));


    LOG_INFO_FILE("Starting NWDAF HTTP Server on port 8080...");
    NWDAFServer server(8080);
    server.run();

    return 0;
}
