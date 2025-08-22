#ifndef _INCLUDE_ANALYTICS_EVENT_H
#define _INCLUDE_ANALYTICS_EVENT_H

#include <string>
#include <chrono>

class AnalyticsEvent {
public:
    std::string eventId;       // e.g., LOAD_LEVEL_INFORMATION
    uint64_t timestamp;        // Epoch time    
    std::string subscriptionId;
    std::string payload;       // added payload

    // Constructor without payload (optional)
    AnalyticsEvent(const std::string& id, const std::string& subId)
        : eventId(id),
        timestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())),
        subscriptionId(subId)
    {
    }

    // Constructor with payload
    AnalyticsEvent(const std::string& id, const std::string& subId, const std::string& data)
        : eventId(id),
        timestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())),
        subscriptionId(subId),
        payload(data)
    {
    }

    virtual ~AnalyticsEvent() = default;
};

#endif //_INCLUDE_ANALYTICS_EVENT_H
