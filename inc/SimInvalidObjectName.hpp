#ifndef _SIM_INVALID_NAME_EXCEPTION_H_
#define _SIM_INVALID_NAME_EXCEPTION_H_

#include "SimException.hpp"
#include "Smp/InvalidObjectName.h"


namespace SimExceptions
{
    class SimInvalidObjectName : public SimException, public virtual Smp::InvalidObjectName 
    {
        public:

        SimInvalidObjectName(Smp::String8 InvalidName, Smp::IObject* Sender = nullptr);

        /// Implements the C++ standard interface for exceptions.
        /// @return  Name of the exception class.
        const Smp::Char8* what() const noexcept override {
            return "SimInvalidObjectName";
        }

        /// Get the invalid object name passed to the constructor of the object.
        /// @return  Invalid object name passed to the constructor of the
        /// object.
        Smp::String8 GetInvalidName() const noexcept override;
        
        private:

        const Smp::String8 InvalidName_;
        //("ABCDEFGHIJKLMNOPQRSTUVWXYZ" +"abcdefghijklmnopqrstuvwxyz" +"0123456789" +"_[]");

    }; // Class: SimInvalidObjectName
}; // Namespace: SimExceptions

#endif // _SIM_INVALID_NAME_EXCEPTION_H_