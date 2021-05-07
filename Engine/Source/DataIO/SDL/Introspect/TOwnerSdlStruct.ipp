#pragma once

#include "DataIO/SDL/Introspect/TOwnerSdlStruct.h"
#include "DataIO/SDL/Introspect/SdlInputContext.h"
#include "DataIO/SDl/Introspect/SdlField.h"
#include "DataIO/SDL/Introspect/field_set_op.h"

#include <utility>
#include <type_traits>
#include <memory>

namespace ph
{

template<typename StructType, typename FieldSet>
inline TOwnerSdlStruct<StructType, FieldSet>::TOwnerSdlStruct(std::string name) :
	SdlStruct(std::move(name))
{}

template<typename StructType, typename FieldSet>
inline void TOwnerSdlStruct<StructType, FieldSet>::fromSdl(
	StructType&            structObj,
	ValueClauses&          clauses,
	const SdlInputContext& ctx) const
{
	// Tolerate unused clauses since structs are often embeded in resource 
	// classes and an excess amount of clauses can be expected
	//
	field_set_op::load_fields_from_sdl_with_redundant_clauses(
		structObj,
		m_fields,
		clauses,
		ctx,
		[](std::string noticeMsg, EFieldImportance importance)
		{
			if(importance == EFieldImportance::OPTIONAL || importance == EFieldImportance::NICE_TO_HAVE)
			{
				logger.log(ELogLevel::NOTE_MED, noticeMsg);
			}
			else
			{
				logger.log(ELogLevel::WARNING_MED, noticeMsg);
			}
		});
}

template<typename StructType, typename FieldSet>
template<typename T>
inline auto TOwnerSdlStruct<StructType, FieldSet>::addField(T field)
	-> TOwnerSdlStruct&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(field));

	return *this;
}

template<typename StructType, typename FieldSet>
inline std::size_t TOwnerSdlStruct<StructType, FieldSet>::numFields() const
{
	return m_fields.numFields();
}

template<typename StructType, typename FieldSet>
inline const SdlField* TOwnerSdlStruct<StructType, FieldSet>::getField(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename StructType, typename FieldSet>
inline const FieldSet& TOwnerSdlStruct<StructType, FieldSet>::getFields() const
{
	return m_fields;
}

}// end namespace ph
