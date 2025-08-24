#ifndef IEVENT_REPOSITORY_H
#define IEVENT_REPOSITORY_H


#include <vector>
#include "../collection/AnalyticsEvent.h"
#include <memory>

class IEventRepository {
public:
	virtual ~IEventRepository() = default;
	virtual void insertBatch(const std::vector<std::shared_ptr<AnalyticsEvent>>& events) = 0;
	virtual void insertEvent(const AnalyticsEvent& e) = 0;

};


#endif // IEVENT_REPOSITORY_H