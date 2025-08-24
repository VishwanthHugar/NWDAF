#include "3rdparty/httplib.h"
#include "NWDAF.h"
#include "SafeQueue.h"
#include "Logger.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <regex>
#include <cstdlib>
#include <memory>

// ---------------- NWDAFServer Methods ----------------
NWDAFServer::NWDAFServer(int Port)
    : mPort(Port)
    , workerPool(std::thread::hardware_concurrency())
    , dispatcherPool(2, 4, workerPool)
{
}

void NWDAFServer::run() {
    // Start Dispatcher
    dispatcherPool.start();

    // Register HTTP handlers
    mServer.Post("/nwdaf-analytics/v1/Nnwdaf_AnalyticsSubscription_Subscribe",
        [this](const httplib::Request& req, httplib::Response& res) { handleSubscribe(req, res); });
    
    mServer.Post("/nwdaf-analytics/v1/Nnwdaf_AnalyticsSubscription_Unsubscribe",
        [this](const httplib::Request& req, httplib::Response& res) { handleUnsubscribe(req, res); });
    
    mServer.Get("/nwdaf-analytics/v1/Nnwdaf_AnalyticsInfo_Request",
        [this](const httplib::Request& req, httplib::Response& res) { handleAnalyticsInfoRequest(req, res); });

    
    // Background notifier thread
    std::thread notifier([this]() { backgroundNotifier(); });
    notifier.detach();

    std::cout << "[NWDAF] Server running at http://localhost:" << mPort << std::endl;
    mServer.listen("0.0.0.0", mPort);
}

// ---------------- HTTP Handlers ----------------
void NWDAFServer::handleSubscribe(const httplib::Request& req, httplib::Response& res) {
    auto reqCopy = std::make_shared<httplib::Request>(req);
    dispatcherPool.submit([this, reqCopy, &res](ThreadPool& wp) {
        processSubscribe(*reqCopy);
        }, 10); // high priority
    res.status = 202;
    res.set_content(R"({"status":"processing"})", "application/json");
}

void NWDAFServer::handleUnsubscribe(const httplib::Request& req, httplib::Response& res) {
    auto reqCopy = std::make_shared<httplib::Request>(req);
    dispatcherPool.submit([this, reqCopy, &res](ThreadPool& wp) {
        processUnsubscribe(*reqCopy);
        }, 5); // medium priority
    res.status = 202;
    res.set_content(R"({"status":"processing"})", "application/json");
}

void NWDAFServer::handleAnalyticsInfoRequest(const httplib::Request& req, httplib::Response& res) {
    auto reqCopy = std::make_shared<httplib::Request>(req);
    dispatcherPool.submit([this, reqCopy, &res](ThreadPool& wp) {
        processAnalyticsInfo(*reqCopy, res);
        }, 1); // low priority
    res.status = 202;
    res.set_content(R"({"status":"processing"})", "application/json");

}



void NWDAFServer::processSubscribe(const httplib::Request& req) {
    std::string subId;
    std::string notifUri;
    int statusCode = 0;
    json callbackResponse;

    try {
        json body = json::parse(req.body);
        subId = body.value("subscriptionId", "");
        notifUri = body.value("notifUri", "");
        std::string eventId = body.value("eventId", "");

        
        if (subId.empty() || notifUri.empty() || eventId.empty()) {
            statusCode = 400;
            callbackResponse = { {"error", "subscriptionId, notifUri and eventId required"} };

        }
        else {
            SubscriptionInfo info;
            info.subscriptionId = subId;
            info.notifUri = notifUri;
            info.eventId = eventId;
            info.analyticsFilter = body.value("analyticsFilter", json::object());

            if (!subscriptionManager.addSubscription(subId, info)) {
                statusCode = 200;
                callbackResponse = { {{"subscriptionId", subId}, "subscription already exists"} };            
            }
            else {
                statusCode = 200;
                callbackResponse = { {"subscriptionId", subId}, {"status", "ACTIVE"} };
            }
            std::cout << "[NWDAF] Subscription processed: " << subId
                << " from " << req.remote_addr << std::endl;
        }
        // ---------------- Asynchronous callback ----------------
        sendCallback(subId, notifUri, statusCode, callbackResponse.dump());

    }
    catch (const std::exception& e) {
        std::cout << "[NWDAF] Subscription failed: invalid JSON: " << e.what() << std::endl;
    }
}

// ---------------- Helper function to send async HTTP callback ----------------
void NWDAFServer::sendCallback(const std::string& subId, const std::string& notifUri, int statusCode, const std::string& payload) {
    
    auto [host, path] = parseNotifUri(notifUri);

    std::thread([subId, notifUri, statusCode, payload, host, path ]() {
        // Extract host and port
        std::smatch match;
        int port = 80;


        std::regex url_regex(R"(http://([^:/]+)(?::(\d+))?(/.*)?)");
        if (std::regex_match(notifUri, match, url_regex)) {
            port = std::stoi(match[2]);

        }
        else {
            std::cout << "Invalid URL\n";
        }

        httplib::Client cli(host.c_str());
        cli.set_keep_alive(true);
        cli.set_read_timeout(5, 0);
        cli.set_write_timeout(5, 0);

        auto res = cli.Post(path.c_str(), payload, "application/json");

        if (res && res->status == 200) {
            std::cout << "[NWDAF] Callback sent successfully to " << notifUri
                << " for subscription " << subId
                << " response code: " << statusCode << std::endl;
        }
        else {
            std::cout << "[NWDAF] Failed to send callback to " << notifUri
                << " for subscription " << subId
                << " response code: " << statusCode << std::endl;
        }
        }).detach();
}


void NWDAFServer::processUnsubscribe(const httplib::Request& req) {
    std::string subId;
    std::string notifUri;

    int statusCode = 0;
    json callbackResponse;

    try {
        json body = json::parse(req.body);
        notifUri = body.value("notifUri", "");

        std::string subId = body.value("subscriptionId", "");
        if (subId.empty()) {
            statusCode = 200;
            callbackResponse = { {"success", "subscription already exists"} };
            sendCallback(subId, notifUri, statusCode, callbackResponse.dump());

            return;
        }
        if (subscriptionManager.removeSubscription(subId)) {
            statusCode = 200;
            callbackResponse = { {"success", "subscription removed"} };
            std::cout << "[NWDAF] Subscription removed: " << subId
                << " by " << req.remote_addr << std::endl;
        }
        else {
            statusCode = 200;
            callbackResponse = { {"success", "subscription already exists"} };
        }
        sendCallback(subId, notifUri, statusCode, callbackResponse.dump());

    }
    catch (...) {
        std::cout << "[NWDAF] Subscription failed: invalid JSON: " << std::endl;
    }
}

void NWDAFServer::processAnalyticsInfo(const httplib::Request& req, httplib::Response& res) {
    try {


        std::string subId;
        if (req.has_param("subscriptionId")) {            
            subId = req.get_param_value("subscriptionId");
            std::cout << "[NWDAF] Nnwdaf_AnalyticsInfo_Request: " << subId
                << " by " << req.remote_addr << std::endl;
            sendAnalyticsInfoNotification(subId);
        }
    }
    catch (...) {
        
    }
}

void NWDAFServer::sendAnalyticsInfoNotification(const std::string& subscriptionId) {
    auto subs = subscriptionManager.getSubscription(subscriptionId);

    if (subs.has_value()){
        SubscriptionInfo info = subs.value();
        auto [host, path] = parseNotifUri(info.notifUri);
        if (host.empty()) {
            std::cout << "[NWDAF] Invalid notifUri for sub " << info.subscriptionId << std::endl;
            return;
        }
        json analyticsInfo = {
            {"subscriptionId", info.subscriptionId},
            {"eventId", info.eventId},
            {"analytics", {
                 {"prediction", "Anomaly Detection"},
                 {"confidence", 0.75},
                 {"timestamp", "2025-08-21T08:15:00Z"}
            }}
        };
        httplib::Client cli(host.c_str());

        cli.set_keep_alive(true);      // reuse TCP connection
        cli.set_read_timeout(5, 0);
        cli.set_write_timeout(5, 0);

        auto res = cli.Post(path.c_str(), analyticsInfo.dump(), "application/json");
        if (res && res->status == 200) {
            std::cout << "[NWDAF] Notification sent to " << info.notifUri
                << " for " << info.subscriptionId << analyticsInfo.dump() << std::endl;
        }
        else {
            std::cout << "[NWDAF] Failed to notify " << info.notifUri
                << " for " << info.subscriptionId << std::endl;
        }
    }
}
void NWDAFServer::processNotification(const SubscriptionInfo& info) {
    json analytics = {
        {"subscriptionId", info.subscriptionId},
        {"eventId", info.eventId},
        {"prediction", "HIGH_CONGESTION"},
        {"confidence", 0.7 + (rand() % 30) / 100.0},
        {"timestamp", "2025-08-21T08:15:00Z"}
    };

    auto [host, path] = parseNotifUri(info.notifUri);
    if (host.empty()) {
        std::cout << "[NWDAF] Invalid notifUri for sub " << info.subscriptionId << std::endl;
        return;
    }

    httplib::Client cli(host.c_str());

    cli.set_keep_alive(true);      // reuse TCP connection
    cli.set_read_timeout(5, 0);
    cli.set_write_timeout(5, 0);

    auto res = cli.Post(path.c_str(), analytics.dump(), "application/json");
    if (res && res->status == 200) {
        std::cout << "[NWDAF] Notification sent to " << info.notifUri
            << " for " << info.subscriptionId << std::endl;
    }
    else {
        std::cout << "[NWDAF] Failed to notify " << info.notifUri
            << " path " << path.c_str() << " "
            << " for " << info.subscriptionId << std::endl;
    }
}

// ---------------- Background Notifier ----------------
void NWDAFServer::backgroundNotifier() {
    while (true) {
        auto subs = subscriptionManager.getAllSubscriptions();
        for (const auto& [subId, info] : subs) {
            dispatcherPool.submit([this, info](ThreadPool& wp) {
                processNotification(info);
                }, 5);
            //mQueue.push({ TaskType::NOTIFY, nullptr, nullptr, info });
        }
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

// ---------------- Utility ----------------
std::pair<std::string, std::string> NWDAFServer::parseNotifUri(const std::string& notifUri) {
    std::regex url_regex(R"(http://([^/]+)(/.*)?)");
    std::smatch match;
    if (std::regex_match(notifUri, match, url_regex)) {
        std::string host = match[1];
        std::string path = match[2].str();
        if (path.empty()) path = "/";
        return { host, path };
    }
    return { "", "" };
}
