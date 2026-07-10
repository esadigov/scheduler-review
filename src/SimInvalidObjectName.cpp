#include "SimInvalidObjectName.hpp"
#include <string.h>

SimExceptions::SimInvalidObjectName::SimInvalidObjectName(Smp::String8 InvalidName, Smp::IObject* Sender)
    :SimException(
        "SimInvalidObjectName"
        ,"Invalid object name"
        ,"Failed to create object because the provided object name does not satisfy the naming rules."
        ,Sender
    )
    ,InvalidName_(InvalidName_) {
}

/// Get the invalid object name passed to the constructor of the object.
/// @return  Invalid object name passed to the constructor of the
/// object.
Smp::String8 SimExceptions::SimInvalidObjectName::GetInvalidName() const noexcept {
    return InvalidName_;
}
