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
template<typename T>
inline auto TOwnerSdlStruct<StructType, FieldSet>::addField(T sdlField)
	-> TOwnerSdlStruct&
{
	// More restrictions on the type of T may be imposed by FieldSet
	static_assert(std::is_base_of_v<SdlField, T>,
		"T is not a SdlField thus cannot be added.");

	m_fields.addField(std::move(sdlField));

	return *this;
}

template<typename StructType, typename FieldSet>
template<typename T>
inline auto TOwnerSdlStruct<StructType, FieldSet>::addStruct(const T& sdlStruct)
	-> TOwnerSdlStruct&
{
	static_assert(std::is_base_of_v<SdlStruct, T>,
		"T is not a SdlStruct thus its fields cannot be added.");



	m_fields.addField(std::move(sdlField));

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
