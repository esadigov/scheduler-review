#include "SimInvalidEventId.hpp"

SimExceptions::SimInvalidEventId::SimInvalidEventId(const Smp::Services::EventId& invalidEventId, Smp::IObject* Sender) noexcept
    :SimException("SimInvalidEventId"
        ,"The Event ID is invalid."
        ,"Check the code XD."
        ,Sender
    )
    ,InvalidEventId_(invalidEventId)
    {
}

/// Get the invalid event identifier.
/// @return  Invalid event identifier.
Smp::Services::EventId SimExceptions::SimInvalidEventId::GetInvalidEventId() const noexcept{
    return InvalidEventId_;
}