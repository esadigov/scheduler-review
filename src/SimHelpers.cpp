#include "SimHelpers.hpp"



bool is_object_name_valid(Smp::String8 Name) {
    if(Name == nullptr) return false;

    if(strlen(Name) == 0) return false;
    if(strchr("0123456789_[]", Name[0]) != nullptr) return false;

    else if(
        strspn(Name, 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_[]")
        != strlen(Name)
    ) return false;
    
}