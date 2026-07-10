#ifndef _SIM_SERVICE_H_
#define _SIM_SERVICE_H_

#include "SimComponent.hpp"
#include "IService.h"

class SimService : public SimComponent, public virtual Smp::IService
{
    public:

    virtual ~SimService() noexcept = default;

    SimService(Smp::String8 ObjectName) : SimComponent(ObjectName) {}
};

#endif // _SIM_SERVICE_H_