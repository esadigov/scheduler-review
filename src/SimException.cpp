#include "SimException.hpp"

SimExceptions::SimException::SimException(Smp::String8 Name
    ,Smp::String8 Description
    ,Smp::String8 Message
    ,Smp::IObject* Sender
    )
    :Name_(Name)
    ,Description_(Description)
    ,Message_(Message)
    ,Sender_(Sender) {

}

/// Returns the name of the exception class. 
/// This name can be used e.g. for debugging purposes.
/// @return  Name of the exception class.
Smp::String8 SimExceptions::SimException::GetName() const noexcept {
    return Name_;
}

/// Returns a textual description of the exception class. This
/// description can be used e.g. for debugging purposes.
/// @return  Textual description of the exception class.
Smp::String8 SimExceptions::SimException::GetDescription() const noexcept {
    return Description_;
}

/// Returns the description of the problem encountered. This message
/// can be used e.g. for debugging purposes.
/// @return  Textual description of the problem encountered.
Smp::String8 SimExceptions::SimException::GetMessage() const noexcept {
    return Message_;
}

/// Returns the sender of the exception instance. This object (and its
/// name and path) can be used e.g. for debugging purposes.
/// @return  Object that emitted the exception.
const Smp::IObject* SimExceptions::SimException::GetSender() const noexcept {
    return Sender_;
}