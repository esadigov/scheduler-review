#ifndef _SIM_INVALID_EVENT_ID_HPP_
#define _SIM_INVALID_EVENT_ID_HPP_

#include "SimException.hpp"
#include "Smp/Services/InvalidEventId.h"

namespace SimExceptions
{
    class SimInvalidEventId : public SimException, public virtual Smp::Services::InvalidEventId
    {
        public:

        SimInvalidEventId(const Smp::Services::EventId& invalidEventId, Smp::IObject* Sender = nullptr) noexcept;

        /// Get the invalid event identifier.
        /// @return  Invalid event identifier.
        Smp::Services::EventId GetInvalidEventId() const noexcept;

        private:

        const Smp::Services::EventId InvalidEventId_;

    }; // Class: InvalidEventId
} // Namespace: SimExceptions

#endif // _SIM_INVALID_EVENT_ID_HPP_