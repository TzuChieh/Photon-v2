#pragma once

#include "DataIO/SDL/Introspect/SdlStructFieldStump.h"
#include "Common/assertion.h"
#include "DataIO/SDL/Introspect/TSdlNestedField.h"
#include "DataIO/SDL/Introspect/SdlStruct.h"

#include <type_traits>
#include <utility>
#include <memory>

namespace ph
{

template<typename StructType, typename Owner>
inline auto SdlStructFieldStump::genFieldSet(StructType Owner::* const structObjPtr) const
	-> TBasicSdlFieldSet<TOwnedSdlField<Owner>>
{
	// TODO: require StructType has getSdlFunction()

	PH_ASSERT(structObjPtr);

	auto const  sdlStruct    = StructType::getSdlStruct();
	const auto& structFields = sdlStruct->getFields();

	TBasicSdlFieldSet<TOwnedSdlField<Owner>> fieldSet;
	for(std::size_t i = 0; i < structFields.numFields(); ++i)
	{
		const auto& structField = structFields[i];

		TSdlNestedField<Owner, StructType> nestedField(
			structObjPtr, 
			&structField);

		fieldSet.addField(nestedField);
	}

	return std::move(fieldSet);
}

}// end namespace ph
