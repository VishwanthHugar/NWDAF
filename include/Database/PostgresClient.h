#ifndef _INCLUDE_POSTGRESCLIENT_H
#define _INCLUDE_POSTGRESCLIENT_H

#include "IDatabaseClient.h"
#include <libpq-fe.h>
#include <mutex>
#include <iostream>

class PostgresClient : public IDatabaseClient {
private:
    PGconn* conn;
    std::mutex dbMutex;

public:
    PostgresClient();
    ~PostgresClient();

    bool connect(const std::string& connStr) override;
    bool insertEvent(const AnalyticsEvent& event) override;
    void disconnect() override;
};

#endif //_INCLUDE_POSTGRESCLIENT_H
