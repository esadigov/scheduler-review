#ifndef _SIM_EXCEPTION_H_
#define _SIM_EXCEPTION_H_

#include "Smp/Exception.h"

namespace SimExceptions
{
    class SimException : public virtual Smp::Exception
    {
        public:

        SimException(Smp::String8 Name
            ,Smp::String8 Description
            ,Smp::String8 Message
            ,Smp::IObject* Sender = nullptr
        );

        virtual const Smp::Char8* what() const noexcept override {
            return "SimException";
        }

        /// Returns the name of the exception class. 
        /// This name can be used e.g. for debugging purposes.
        /// @return  Name of the exception class.
        Smp::String8 GetName() const noexcept override;

        /// Returns a textual description of the exception class. This
        /// description can be used e.g. for debugging purposes.
        /// @return  Textual description of the exception class.
        Smp::String8 GetDescription() const noexcept override;

        /// Returns the description of the problem encountered. This message
        /// can be used e.g. for debugging purposes.
        /// @return  Textual description of the problem encountered.
        Smp::String8 GetMessage() const noexcept override;

        /// Returns the sender of the exception instance. This object (and its
        /// name and path) can be used e.g. for debugging purposes.
        /// @return  Object that emitted the exception.
        const Smp::IObject* GetSender() const noexcept override;

        private:

        Smp::String8 Name_;
        Smp::String8 Description_;
        Smp::String8 Message_;
        Smp::IObject* Sender_;
    }; // Class: SimException
}; // Namespace: SimExceptions

#endif // _SIM_EXCEPTION_H_