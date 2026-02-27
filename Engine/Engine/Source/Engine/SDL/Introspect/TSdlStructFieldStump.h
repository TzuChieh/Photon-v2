#pragma once

#include "Engine/SDL/Introspect/FieldSet/TSdlDefaultFieldSet.h"
#include "Engine/SDL/Introspect/TSdlOwnedField.h"

namespace ph
{

/*! @brief Information required for adding struct members to another struct-like type.
*/
template<typename Owner>
class TSdlStructFieldStump final
{
public:
	template<typename FieldSet, typename StructType>
	auto genFieldSet(StructType Owner::* structObjPtr) const
	-> FieldSet;

	// TODO: funcs for optional prefix names or some override properties?

};

}// end namespace ph

#include "Engine/SDL/Introspect/TSdlStructFieldStump.ipp"
