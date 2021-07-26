#pragma once

#include "DataIO/SDL/Introspect/TOwnerSdlStruct.h"
#include "DataIO/SDl/Introspect/SdlField.h"
#include "DataIO/SDL/Introspect/SdlStructFieldStump.h"
#include "Common/assertion.h"

#include <utility>
#include <type_traits>
#include <memory>

namespace ph
{

template<typename StructType>
inline TOwnerSdlStruct<StructType>::TOwnerSdlStruct(std::string name) :

	SdlStruct(std::move(name)),

	m_fields()
{}

template<typename StructType>
template<typename T>
inline auto TOwnerSdlStruct<StructType>::addField(T sdlField)
	-> TOwnerSdlStruct&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename StructType>
template<typename StructObjType>
inline auto TOwnerSdlStruct<StructType>::addStruct(StructObjType StructType::* const structObjPtr)
	-> TOwnerSdlStruct&
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
inline auto TOwnerSdlStruct<StructType>::addStruct(
	StructObjType StructType::* const structObjPtr,
	const SdlStructFieldStump&        structFieldStump)

	-> TOwnerSdlStruct&
{
	// More restrictions on StructObjType may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlStruct, StructObjType>,
		"StructObjType is not a SdlStruct thus cannot be added.");

	PH_ASSERT(structObjPtr);

	m_fields.addFields(structFieldStump.genFieldSet(structObjPtr));

	return *this;
}

template<typename StructType>
inline std::size_t TOwnerSdlStruct<StructType>::numFields() const
{
	return m_fields.numFields();
}

template<typename StructType>
inline const SdlField* TOwnerSdlStruct<StructType>::getField(const std::size_t index) const
{
	return m_fields.getField(index);
}

template<typename StructType>
inline auto TOwnerSdlStruct<StructType>::getFields() const
	-> const TBasicSdlFieldSet<TOwnedSdlField<StructType>>&
{
	return m_fields;
}

template<typename StructType>
inline auto TOwnerSdlStruct<StructType>::description(std::string descriptionStr)
	-> TOwnerSdlStruct&
{
	setDescription(std::move(descriptionStr));
	return *this;
}

}// end namespace ph
