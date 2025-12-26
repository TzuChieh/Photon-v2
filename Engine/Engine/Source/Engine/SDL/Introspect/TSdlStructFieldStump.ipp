#pragma once

#include "Engine/SDL/Introspect/TSdlStructFieldStump.h"
#include "Engine/SDL/Introspect/TSdlNestedField.h"
#include "Engine/SDL/Introspect/SdlStruct.h"

#include <Common/assertion.h>

#include <type_traits>
#include <utility>
#include <memory>

namespace ph
{

template<typename Owner>
template<typename StructType>
inline auto TSdlStructFieldStump<Owner>::genFieldSet(StructType Owner::* const structObjPtr) const
-> TSdlBruteForceFieldSet<TSdlOwnedField<Owner>>
{
	// TODO: require StructType has getSdlFunction()

	PH_ASSERT(structObjPtr);

	auto const  sdlStruct    = StructType::getSdlStruct();
	const auto& structFields = sdlStruct->getFields();

	TSdlBruteForceFieldSet<TSdlOwnedField<Owner>> fieldSet;
	for(std::size_t i = 0; i < structFields.numFields(); ++i)
	{
		const auto& structField = structFields[i];

		TSdlNestedField<Owner, StructType> nestedField(
			structObjPtr, 
			&structField);

		fieldSet.addField(nestedField);
	}

	return fieldSet;
}

}// end namespace ph
