#include "SimException.hpp"
#include "SimInvalidCycleTime.hpp"


SimExceptions::SimInvalidCycleTime::SimInvalidCycleTime(Smp::IObject* Sender)
    : SimException("SimInvalidCycleTime"
        ,"The cycle time is invalid."
        ,"Check the requirements."
        ,Sender
    ) {
}