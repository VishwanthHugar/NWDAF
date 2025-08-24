#ifndef _INCLUDE_COLLECTORS_H
#define _INCLUDE_COLLECTORS_H

#include "INetworkFunction.h"
#include "AnalyticsEvent.h"
#include "../Database/IEventRepository.h"     // Repository for DB insertion
#include "../3rdParty/httplib.h"  // real HTTP client
#include <memory>
#include <string>
#include <chrono>
#include <iostream>

// Helper function: HTTP GET request to simulator
inline std::string httpGetFromSimulator(const std::string& host, int port, const std::string& path) {
    httplib::Client cli(host, port);

    cli.set_keep_alive(true);      // reuse TCP connection
    cli.set_read_timeout(5, 0);
    cli.set_write_timeout(5, 0);

    auto res = cli.Get(path.c_str());
    if (res && res->status == 200) {
        return res->body;
    }
    else {
        std::cerr << "HTTP GET failed: " << path << "\n";
        return ""; // fallback empty JSON
    }
}

// ---------------- Base Collector with DB integration ----------------
class BaseCollector : public INetworkFunction {
protected:
    std::shared_ptr<IEventRepository> repository; // handles DB insertion
public:
    explicit BaseCollector(std::shared_ptr<IEventRepository> repo) : repository(repo) {}
    virtual ~BaseCollector() = default;
};

// ---------------- AMF Collector ----------------
class AMFCollector : public BaseCollector {
public:
    using BaseCollector::BaseCollector; // inherit constructor

    std::shared_ptr<AnalyticsEvent> collectData() override {
        std::string payload = httpGetFromSimulator("localhost", 8081, "/amf/v1/ue-connections");
        if (payload.empty())
            return nullptr;

        auto event = std::make_shared<AnalyticsEvent>("UE_CONNECTION_INFO", "sub_amf_001", payload);
        //if (repository) {
        //    repository->insertEvent(*event); // insert directly into DB
        //}
        return event;
    }
};

// ---------------- SMF Collector ----------------
class SMFCollector : public BaseCollector {
public:
    using BaseCollector::BaseCollector;

    std::shared_ptr<AnalyticsEvent> collectData() override {
        std::string payload = httpGetFromSimulator("localhost", 8081, "/smf/v1/qos-flows");
        if (payload.empty())
            return nullptr;
        std::cout << "[SMFCollector] the payload \n" << std::endl;
        auto event = std::make_shared<AnalyticsEvent>("QOS_FLOW_INFO", "sub_smf_001", payload);
        //if (repository) {
        //    repository->insertEvent(*event);
        //}
        return event;
    }
};

// ---------------- OAM Collector ----------------
class OAMCollector : public BaseCollector {
public:
    using BaseCollector::BaseCollector;

    std::shared_ptr<AnalyticsEvent> collectData() override {
        std::string payload = httpGetFromSimulator("localhost", 8081, "/oam/v1/historical-load");
        auto event = std::make_shared<AnalyticsEvent>("HISTORICAL_LOAD_INFO", "sub_oam_001", payload);
        if (payload.empty())
            return nullptr;

        //if (repository) {
        //    repository->insertEvent(*event);
        //}
        return event;
    }
};

// ---------------- Slice Collector ----------------
class SliceCollector : public BaseCollector {
public:
    using BaseCollector::BaseCollector;

    std::shared_ptr<AnalyticsEvent> collectData() override {
        std::string payload = httpGetFromSimulator("localhost", 8081, "/slice/v1/resource-usage");
        auto event = std::make_shared<AnalyticsEvent>("SLICE_RESOURCE_INFO", "sub_slice_001", payload);
        if (payload.empty())
            return nullptr;

        //if (repository) {
        //    repository->insertEvent(*event);
        //}
        return event;
    }
};

#endif //_INCLUDE_COLLECTORS_H
