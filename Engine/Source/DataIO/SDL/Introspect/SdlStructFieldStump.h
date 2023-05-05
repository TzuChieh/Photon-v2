#pragma once

#include "DataIO/SDL/Introspect/TSdlBruteForceFieldSet.h"
#include "DataIO/SDL/Introspect/TSdlOwnedField.h"

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

#include "DataIO/SDL/Introspect/SdlStructFieldStump.ipp"
