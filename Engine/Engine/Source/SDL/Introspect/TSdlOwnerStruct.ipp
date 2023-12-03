#pragma once

#include "SDL/Introspect/TSdlOwnerStruct.h"
#include "SDL/Introspect/SdlField.h"
#include "SDL/Introspect/SdlStructFieldStump.h"
#include "Common/assertion.h"

#include <utility>
#include <type_traits>
#include <memory>

namespace ph
{

template<typename StructType>
inline TSdlOwnerStruct<StructType>::TSdlOwnerStruct(std::string name) :

	SdlStruct(std::move(name)),

	m_fields()
{}

template<typename StructType>
template<typename T>
inline auto TSdlOwnerStruct<StructType>::addField(T sdlField)
	-> TSdlOwnerStruct&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename StructType>
template<typename StructObjType>
inline auto TSdlOwnerStruct<StructType>::addStruct(StructObjType StructType::* const structObjPtr)
	-> TSdlOwnerStruct&
{
	// More restrictions on StructObjType may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlStruct, StructObjType>,
		"StructObjType is not a SdlStruct thus cannot be added.");

	PH_ASSERT(structObjPtr);

	m_fields.addFields(SdlStructFieldStump().genFieldSet(structObjPtr));

	return *this;
}

template<typename StructType>
template<typename StructObjType>
inline auto TSdlOwnerStruct<StructType>::addStruct(
	StructObjType StructType::* const structObjPtr,
	const SdlStructFieldStump&        structFieldStump)

	-> TSdlOwnerStruct&
{
	// More restrictions on StructObjType may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlStruct, StructObjType>,
		"StructObjType is not a SdlStruct thus cannot be added.");

	PH_ASSERT(structObjPtr);

	m_fields.addFields(structFieldStump.genFieldSet(structObjPtr));

	return *this;
}

template<typename StructType>
inline void TSdlOwnerStruct<StructType>::initDefaultStruct(StructType& structObj) const
{
	setFieldsToDefaults(structObj);
}

template<typename StructType>
inline std::size_t TSdlOwnerStruct<StructType>::numFields() const
{
	return m_fields.numFields();
}

template<typename StructType>
inline const SdlField* TSdlOwnerStruct<StructType>::getField(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename StructType>
inline auto TSdlOwnerStruct<StructType>::getFields() const
	-> const TSdlBruteForceFieldSet<TSdlOwnedField<StructType>>&
{
	return m_fields;
}

template<typename StructType>
inline auto TSdlOwnerStruct<StructType>::description(std::string descriptionStr)
	-> TSdlOwnerStruct&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

template<typename StructType>
inline void TSdlOwnerStruct<StructType>::setFieldsToDefaults(StructType& structObj) const
{
	for(std::size_t fieldIdx = 0; fieldIdx < m_fields.numFields(); ++fieldIdx)
	{
		const auto& field = m_fields[fieldIdx];

		// Set field to default value regardless of its importance (field importance is for import/export)
		field.ownedValueToDefault(structObj);
	}
}

}// end namespace ph
