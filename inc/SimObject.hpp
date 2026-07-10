#ifndef _SIM_OBJECT_H_
#define _SIM_OBJECT_H_

#include "IObject.h"


class SimObject : public virtual Smp::IObject
{
    public:
    
    virtual ~SimObject() noexcept = default;
    
    SimObject(Smp::String8 ObjectName);

    SimObject(Smp::String8 ObjectName,
        Smp::String8 ObjectDescription);

    SimObject(Smp::String8 ObjectName,
        Smp::String8 ObjectDescription,
        Smp::IObject* ParrentObject);
    
    SimObject(Smp::String8 ObjectName,
        Smp::String8 ObjectDescription,
        Smp::IObject* ParrentObject,
        Smp::IObject* ChildObject);

    /// Return the name of the object.
    /// Names must <ul> <li>be unique within their context (i.e. for the
    /// same parent),</li> <li>not be empty,</li> <li>start with a letter,
    /// and</li> <li>only contain letters, digits, the underscore ("_") and
    /// brackets ("[" and "]").</li> </ul>
    /// @remarks Applications may display the name as user readable object
    /// identification.
    /// @return  Name of object.
    Smp::String8 GetName() const override;

    /// Return the description of the object ("property getter").
    /// Descriptions are optional and may be empty.
    /// @remarks Applications may display the description as additional
    /// information about the object.
    /// @return  Description of object.
    Smp::String8 GetDescription() const override;

    /// Returns the parent object of the object.
    /// Objects link to their parent to allow traversing the tree of
    /// objects upwards to the root object of a composition.
    /// @remarks Typically, only the simulator itself is a root object, so
    /// all other objects should have a parent object.
    /// @return  Parent object of object or nullptr if object has no parent.
    Smp::IObject* GetParent() const override;

    /// Returns the child object with the given name, or nullptr if no such
    /// object is present among the children of this object.
    /// GetChild can be used while adding elements under this object to
    /// check if a DuplicateName exception has to be raised.
    /// GetChild has to provide all and only the objects that the Resolver
    /// shall resolve. The Resolver can be implemented in terms of a chain
    /// of GetChild and GetParent.
    /// @param   name The name of the requested child object.
    /// @return  The children object with the given name, or nullptr.
    Smp::IObject* GetChild(
        Smp::String8 name) const override;

    private:

    Smp::String8 ObjectName_;
    Smp::String8 ObjectDescription_ = "Default Description";
    Smp::IObject* ParrentObject_{nullptr};
    Smp::IObject* ChildObject_{nullptr};
}; // Class: SimObject

#endif // _SIM_OBJECT_H_