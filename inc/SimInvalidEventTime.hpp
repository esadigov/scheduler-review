#ifndef _SIM_INVALID_EVENT_TIME_H_
#define _SIM_INVALID_EVENT_TIME_H_

#include "SimException.hpp"
#include "Smp/Services/InvalidEventTime.h"

namespace SimExceptions
{
    class SimInvalidEventTime : public SimException, public virtual Smp::Services::InvalidEventTime
    {
        public:

        SimInvalidEventTime(Smp::IObject* Sender = nullptr);

        /// Implements the C++ standard interface for exceptions.
        /// @return  Name of the exception class.
        const Smp::Char8* what() const noexcept override {
            return "InvalidEventTime";
        }

    }; // Class: SimInvalidEventTime
}; // Namespace: SimExceptions


#endif // _SIM_INVALID_EVENT_TIME_H_