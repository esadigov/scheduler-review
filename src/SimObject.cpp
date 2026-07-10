#include "SimObject.hpp"
#include "SimHelpers.hpp"
#include "SimInvalidObjectName.hpp"

/*   [ IObject implementation ]  */

SimObject::SimObject(Smp::String8 ObjectName) {
    if(is_object_name_valid(ObjectName)) {
        ObjectName_ = ObjectName;
    } else {
        throw SimExceptions::SimInvalidObjectName(ObjectName);
    }
}

// i assume you provided valid objects :D
SimObject::SimObject(Smp::String8 ObjectName, 
    Smp::String8 ObjectDescription) : ObjectDescription_(ObjectDescription){
    
    if(is_object_name_valid(ObjectName)) {
        ObjectName_ = ObjectName;
    } else {
        throw SimExceptions::SimInvalidObjectName(ObjectName);
    } 
}

SimObject::SimObject(Smp::String8 ObjectName, 
    Smp::String8 ObjectDescription,
    Smp::IObject* ParrentObject) : ObjectDescription_(ObjectDescription), ParrentObject_(ParrentObject) {
        if(is_object_name_valid(ObjectName)) {
        ObjectName_ = ObjectName;
    } else {
        throw SimExceptions::SimInvalidObjectName(ObjectName);
    } 
}

SimObject::SimObject(Smp::String8 ObjectName, 
    Smp::String8 ObjectDescription,
    Smp::IObject* ParrentObject,
    Smp::IObject* ChildObject) : ObjectDescription_(ObjectDescription), ParrentObject_(ParrentObject), ChildObject_(ChildObject) {
        if(is_object_name_valid(ObjectName)) {
        ObjectName_ = ObjectName;
    } else {
        throw SimExceptions::SimInvalidObjectName(ObjectName);
    } 
}

/// Return the name of the object.
/// Names must <ul> <li>be unique within their context (i.e. for the
/// same parent),</li> <li>not be empty,</li> <li>start with a letter,
/// and</li> <li>only contain letters, digits, the underscore ("_") and
/// brackets ("[" and "]").</li> </ul>
/// @remarks Applications may display the name as user readable object
/// identification.
/// @return  Name of object.
Smp::String8 SimObject::GetName() const {
    return ObjectName_;
}

/// Return the description of the object ("property getter").
/// Descriptions are optional and may be empty.
/// @remarks Applications may display the description as additional
/// information about the object.
/// @return  Description of object.
Smp::String8 SimObject::GetDescription() const {
    return ObjectDescription_;
}

/// Returns the parent object of the object.
/// Objects link to their parent to allow traversing the tree of
/// objects upwards to the root object of a composition.
/// @remarks Typically, only the simulator itself is a root object, so
/// all other objects should have a parent object.
/// @return  Parent object of object or nullptr if object has no parent.
Smp::IObject* SimObject::GetParent() const {
    return ParrentObject_;
}

/// Returns the child object with the given name, or nullptr if no such
/// object is present among the children of this object.
/// GetChild can be used while adding elements under this object to
/// check if a DuplicateName exception has to be raised.
/// GetChild has to provide all and only the objects that the Resolver
/// shall resolve. The Resolver can be implemented in terms of a chain
/// of GetChild and GetParent.
/// @param   name The name of the requested child object.
/// @return  The children object with the given name, or nullptr.
Smp::IObject* SimObject::GetChild(
    Smp::String8 name) const {
    return ChildObject_;
}
