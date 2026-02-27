#pragma once

#include "Engine/SDL/Introspect/FieldSet/TSdlBruteForceFieldSet.h"
#include "Engine/SDL/Introspect/FieldSet/TSdlSortedFieldSet.h"

namespace ph
{

//template<typename BaseFieldType>
//using TSdlDefaultFieldSet = TSdlBruteForceFieldSet<BaseFieldType>;

template<typename BaseFieldType>
using TSdlDefaultFieldSet = TSdlSortedFieldSet<BaseFieldType>;

}// end namespace ph
