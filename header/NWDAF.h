#ifndef NWDAF_SERVER_H
#define NWDAF_SERVER_H

#include "../header/3rdparty/httplib.h"
#include "../header/NWDAF.h"
#include "../header/SafeQueue.h"
#include "../header/Subscription.h"
#include "../header/ThreadPool.h"
#include "../header/ThreadPoolManager.h"

#include <thread>
#include <string>
#include <regex>
#include <memory>
#include <utility>

// ---------------- Task Struct ----------------
enum class TaskType {
    SUBSCRIBE,
    UNSUBSCRIBE,
    ANALYTICS_INFO,
    NOTIFY
};

struct Task {
    TaskType type;
    std::shared_ptr<httplib::Request> req; // incoming HTTP request
    httplib::Response* res;                // response (for HTTP handlers)
    SubscriptionInfo subInfo;              // used for NOTIFY
};

// ---------------- NWDAFServer ----------------
class NWDAFServer {
public:
    explicit NWDAFServer(int mPort = 8080);
    void run();

private:
    int mPort;
    httplib::Server mServer;
    SubscriptionManager subscriptionManager;

    // --------------- Thread pools ---------------
    ThreadPool workerPool;               // Execution req
    ThreadPoolManager dispatcherPool;    // Dispatching req

    // ---------------- HTTP Handlers ----------------
    void handleSubscribe(const httplib::Request& req, httplib::Response& res);
    void handleUnsubscribe(const httplib::Request& req, httplib::Response& res);
    void handleAnalyticsInfoRequest(const httplib::Request& req, httplib::Response& res);

    // ---------------- Queue Worker ----------------
    //void queueWorker();

    // ---------------- Processing Functions ----------------
    void processSubscribe(const httplib::Request& req, httplib::Response& res);
    void processUnsubscribe(const httplib::Request& req, httplib::Response& res);
    void processAnalyticsInfo(const httplib::Request& req, httplib::Response& res);
    void processNotification(const SubscriptionInfo& info);
    void sendAnalyticsInfoNotification(const std::string& subscriptionId);

    // ---------------- Background Notifier ----------------
    void backgroundNotifier();

    // ---------------- Utility ----------------
    std::pair<std::string, std::string> parseNotifUri(const std::string& notifUri);
};

#endif // NWDAF_SERVER_H
