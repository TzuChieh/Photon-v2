#pragma once

#include "Engine/SDL/Introspect/TSdlBruteForceFieldSet.h"
#include "Engine/SDL/Introspect/TSdlOwnedField.h"

namespace ph
{

// Information required for adding the struct members to another struct

class SdlStructFieldStump final
{
public:
	template<typename StructType, typename Owner>
	auto genFieldSet(StructType Owner::* structObjPtr) const
		-> TSdlBruteForceFieldSet<TSdlOwnedField<Owner>>;

	// TODO: funcs for optional prefix names or some override properties?

};

}// end namespace ph

#include "Engine/SDL/Introspect/SdlStructFieldStump.ipp"
