#pragma once

#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"

#include <cstddef>
#include <string>

namespace ph
{

class SdlStructFieldStump;

/*! @brief SDL binding type for a typical C++ struct.
*/
template<typename StructType>
class TOwnerSdlStruct : public SdlStruct
{
public:
	using OwnerType = StructType;

public:
	explicit TOwnerSdlStruct(std::string name);

	/*! @brief Initialize a struct object to default values.
	*/
	void initDefaultStruct(StructType& structObj) const;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;

	template<typename T>
	TOwnerSdlStruct& addField(T sdlField);

	template<typename StructObjType>
	TOwnerSdlStruct& addStruct(StructObjType StructType::* structObjPtr);

	template<typename StructObjType>
	TOwnerSdlStruct& addStruct(
		StructObjType StructType::* structObjPtr,
		const SdlStructFieldStump&  structFieldStump);

	auto getFields() const -> const TBasicSdlFieldSet<TOwnedSdlField<StructType>>&;

	TOwnerSdlStruct& description(std::string descriptionStr);

private:
	/*!
	Set fields of the struct object @p structObj to default values (as described in SDL struct definition).
	*/
	void setFieldsToDefaults(StructType& structObj) const;

	TBasicSdlFieldSet<TOwnedSdlField<StructType>> m_fields;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TOwnerSdlStruct.ipp"
