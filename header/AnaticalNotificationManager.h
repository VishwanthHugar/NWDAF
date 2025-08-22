#ifndef _INCLUDE_ANALYTICAL_NOTIFICATION_MANAGER_H
#define _INCLUDE_ANALYTICAL_NOTIFICATION_MANAGER_H

#include "NotificationManagerBase.h"
#incldie "IObserver.h"
#include <vector>
#include <memory>
#include <mutex>
#include <algorithm>

class AnalyticalNotificationManager : public NotificationManagerBase {
private:
    std::vector<std::shared_ptr<IObserver>> observers_;
    std::vector<std::shared_ptr<AnalyticsEventBase>> notifications_;
    mutable std::mutex mMtx;

public:
    AnalyticalNotificationManager() = default;
    ~AnalyticalNotificationManager() = default;

    void addObserver(std::shared_ptr<IObserver> obs) override {
        std::lock_guard<std::mutex> lk(mMtx);
        observers_.push_back(obs);
    }

    void removeObserver(std::shared_ptr<IObserver> obs) override {
        std::lock_guard<std::mutex> lk(mMtx);
        observers_.erase(std::remove(observers_.begin(), observers_.end(), obs), observers_.end());
    }

    void addNotification(std::shared_ptr<AnalyticsEventBase> notif) override {
        {
            std::lock_guard<std::mutex> lk(mMtx);
            notifications_.push_back(notif);
        }
        notifyObservers(*notif);
    }

    std::vector<std::shared_ptr<AnalyticsEventBase>> getNotifications() const override {
        std::lock_guard<std::mutex> lk(mMtx);
        return notifications_;
    }

protected:
    void notifyObservers(const AnalyticsEventBase& notif) override {
        std::lock_guard<std::mutex> lk(mMtx);
        for (auto& obs : observers_) {
            obs->update(notif);
        }
    }
};

#endif //_INCLUDE_ANALYTICAL_NOTIFICATION_MANAGER_H
