#pragma once

#include <string.h>
#include <stdlib.h>
#include "Smp/String8.h"


/// Return the name of the object.
/// Names must <ul> <li>be unique within their context (i.e. for the
/// same parent),</li> <li>not be empty,</li> <li>start with a letter,
/// and</li> <li>only contain letters, digits, the underscore ("_") and
/// brackets ("[" and "]").</li> </ul>
/// @param Name Name of the object
/// @return It returns true if it complies with the above rules, else false
bool is_object_name_valid(Smp::String8 Name);