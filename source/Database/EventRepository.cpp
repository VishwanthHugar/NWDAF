#include "EventRepository.h"
#include <iostream>

EventRepository::EventRepository(std::shared_ptr<IDatabaseClient> dbClient)
    : client(std::move(dbClient)) {
}

void EventRepository::insertBatch(const std::vector<std::shared_ptr<AnalyticsEvent>>& events) {
    std::lock_guard<std::mutex> lock(repoMutex);

    if (!client) {
        std::cerr << "No DB client available.\n";
        return;
    }

    for (const auto& event : events) {
        if (!client->insertEvent(*event)) {
            std::cerr << "Failed to insert event of type: "
                << event->eventId << "\n";
        }
    }
}
