#ifndef POSTGRES_EVENT_REPOSITORY_H
#define POSTGRES_EVENT_REPOSITORY_H

#include "IEventRepository.h"
#include "PostgresClient.h"
#include <vector>
#include <memory>
#include <string>

class PostgresEventRepository : public IEventRepository {
public:
    explicit PostgresEventRepository(const std::string& connStr);
    ~PostgresEventRepository() override;

    void insertBatch(const std::vector<std::shared_ptr<AnalyticsEvent>>& events) override;
    void insertEvent(const AnalyticsEvent& e) override;

private:
    std::shared_ptr<PostgresClient> client;
};

#endif // POSTGRES_EVENT_REPOSITORY_H
