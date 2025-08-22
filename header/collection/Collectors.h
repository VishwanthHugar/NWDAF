#ifndef _INCLUDE_COLLECTORS_H
#define _INCLUDE_COLLECTORS_H

#include "INetworkFunction.h"
#include "AnalyticsEvent.h"
#include "../3rdParty/httplib.h"   // real HTTP client
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
        return "{}"; // fallback empty JSON
    }
}

// AMF Collector
class AMFCollector : public INetworkFunction {
public:
    std::shared_ptr<AnalyticsEvent> collectData() override {
        std::string payload = httpGetFromSimulator("localhost", 8081, "/amf/v1/ue-connections");
        return std::make_shared<AnalyticsEvent>("UE_CONNECTION_INFO", "sub_amf_001", payload);
    }
};

// SMF Collector
class SMFCollector : public INetworkFunction {
public:
    std::shared_ptr<AnalyticsEvent> collectData() override {
        std::string payload = httpGetFromSimulator("localhost", 8081, "/smf/v1/qos-flows");
        return std::make_shared<AnalyticsEvent>("QOS_FLOW_INFO", "sub_smf_001", payload);
    }
};

// OAM Collector
class OAMCollector : public INetworkFunction {
public:
    std::shared_ptr<AnalyticsEvent> collectData() override {
        std::string payload = httpGetFromSimulator("localhost", 8081, "/oam/v1/historical-load");
        return std::make_shared<AnalyticsEvent>("HISTORICAL_LOAD_INFO", "sub_oam_001", payload);
    }
};

// Slice Collector
class SliceCollector : public INetworkFunction {
public:
    std::shared_ptr<AnalyticsEvent> collectData() override {
        std::string payload = httpGetFromSimulator("localhost", 8081, "/slice/v1/resource-usage");
        return std::make_shared<AnalyticsEvent>("SLICE_RESOURCE_INFO", "sub_slice_001", payload);
    }
};

#endif //_INCLUDE_COLLECTORS_H
