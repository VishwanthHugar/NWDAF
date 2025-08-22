#ifndef _INCLUDE_ISUBJECT_H
#define _INCLUDE_ISUBJECT_H

#include "IObserver.h"
#include <vector>
#include <memory>

class ISubject {
public:
    virtual void attach(std::shared_ptr<IObserver> observer) = 0;
    virtual void detach(std::shared_ptr<IObserver> observer) = 0;
    virtual void notify(const KPIData& data) = 0;
    virtual ~ISubject() = default;
};

#endif //_INCLUDE_ISUBJECT_H
