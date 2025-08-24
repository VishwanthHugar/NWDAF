#ifndef EVENT_REPOSITORY_H
#define EVENT_REPOSITORY_H

#include "IEventRepository.h"
#include "IDatabaseClient.h"
#include <memory>
#include <mutex>

class EventRepository : public IEventRepository {
public:
    explicit EventRepository(std::shared_ptr<IDatabaseClient> dbClient);

    void insertBatch(const std::vector<std::shared_ptr<AnalyticsEvent>>& events) override;

private:
    std::shared_ptr<IDatabaseClient> client;
    std::mutex repoMutex;  // ensure thread-safety at repo level
};

#endif // EVENT_REPOSITORY_H
