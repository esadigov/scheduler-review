#include "SimException.hpp"
#include "SimInvalidEventTime.hpp"


SimExceptions::SimInvalidEventTime::SimInvalidEventTime(Smp::IObject* Sender)
    : SimException("SimInvalidEventTime"
        ,"Invalid event time."
        ,"Event time does not meet the scheduling requirements."
        ,Sender
    ) {
}