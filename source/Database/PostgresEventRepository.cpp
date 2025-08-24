#include "PostgresEventRepository.h"
#include <iostream>
#include <stdexcept>

PostgresEventRepository::PostgresEventRepository(const std::string& connStr)
    : client(std::make_shared<PostgresClient>())
{

    if (!client->connect(connStr)) {
        throw std::runtime_error("Failed to connect to Postgres DB");
    }
}

PostgresEventRepository::~PostgresEventRepository() {
    if (!client)
        return;

    client->disconnect();
}

void PostgresEventRepository::insertBatch(const std::vector<std::shared_ptr<AnalyticsEvent>>& events) {
    if (!client)
        return;
    std::cout << "[PostgresEventRepository::insertBatch] Enter " << std::endl;
    for (const auto& e : events) {
        if (!client->insertEvent(*e)) {
            std::cerr << "Failed to insert event: " << e->eventId << std::endl;
        }
    }
}

void PostgresEventRepository::insertEvent(const AnalyticsEvent& e) {
    insertBatch({ std::make_shared<AnalyticsEvent>(e) });
}
