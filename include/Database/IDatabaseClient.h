#ifndef _INCLUDE_IDATABASECLIENT_H
#define _INCLUDE_IDATABASECLIENT_H

#include <string>
#include "../collection/AnalyticsEvent.h"

class IDatabaseClient {
public:
    virtual ~IDatabaseClient() = default;

    // Return bool on connect to indicate success
    virtual bool connect(const std::string& connStr) = 0;

    // Insert single event, return bool on success
    virtual bool insertEvent(const AnalyticsEvent& event) = 0;

    // Disconnect from DB
    virtual void disconnect() = 0;
};

#endif //_INCLUDE_IDATABASECLIENT_H
