#include "PostgresClient.h"
#include <iostream>

PostgresClient::PostgresClient() : conn(nullptr) {}

PostgresClient::~PostgresClient() {
    disconnect();
}

bool PostgresClient::connect(const std::string& connStr) {
    std::lock_guard<std::mutex> lock(dbMutex);

    conn = PQconnectdb(connStr.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        std::cerr << "Postgres connection failed: "
            << PQerrorMessage(conn) << std::endl;
        PQfinish(conn);
        conn = nullptr;
        return false;
    }
    std::cout << "Connected to PostgreSQL DB successfully.\n";
    return true;
}

bool PostgresClient::insertEvent(const AnalyticsEvent& event) {
    std::lock_guard<std::mutex> lock(dbMutex);

    if (!conn) {
        std::cerr << "DB not connected!\n";
        return false;
    }

    // Compute epoch milliseconds for ts
    const std::string ts_str = std::to_string(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count()
    );

    std::string query =
        "INSERT INTO analytics_events (event_type, subscription_id, payload, ts) "
        "VALUES ($1, $2, $3, $4);";

    const char* paramValues[4] = {
        event.eventId.c_str(),
        event.subscriptionId.c_str(),
        event.payload.c_str(),
        ts_str.c_str()
    };

    PGresult* res = PQexecParams(
        conn,
        query.c_str(),
        4,            // param count
        nullptr,      // let server infer OIDs
        paramValues,
        nullptr,      // param lengths
        nullptr,      // param formats
        0             // text format
    );
    std::cout << "[PostgresClient] PQexecParams executed .\n";
    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        std::cerr << "Insert failed: " << PQerrorMessage(conn) << std::endl;
        PQclear(res);
        return false;
    }

    PQclear(res);
    return true;
}

void PostgresClient::disconnect() {
    std::lock_guard<std::mutex> lock(dbMutex);

    if (conn) {
        PQfinish(conn);
        conn = nullptr;
        std::cout << "Disconnected from PostgreSQL.\n";
    }
}
