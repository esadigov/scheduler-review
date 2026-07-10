#ifndef _SIM_INVALID_CYCLE_TIME_HPP_
#define _SIM_INVALID_CYCLE_TIME_HPP_

#include "SimException.hpp"
#include "Smp/Services/InvalidCycleTime.h"

namespace SimExceptions
{
    class SimInvalidCycleTime : public SimException, public virtual Smp::Services::InvalidCycleTime
    {
        public:

        SimInvalidCycleTime(Smp::IObject* Sender = nullptr);

        /// Implements the C++ standard interface for exceptions.
        /// @return  Name of the exception class.
        const Smp::Char8* what() const noexcept override {
            return "SimInvalidCycleTime";
        }

    }; // Class: SimInvalid
}; // Namespace: SimExceptions


#endif // _SIM_INVALID_CYCLE_TIME_HPP_