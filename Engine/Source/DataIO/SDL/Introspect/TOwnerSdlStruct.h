#pragma once

#include "DataIO/SDL/Introspect/SdlStruct.h"
#include "DataIO/SDL/Introspect/TBasicSdlFieldSet.h"
#include "DataIO/SDL/Introspect/TOwnedSdlField.h"

#include <cstddef>
#include <string>

namespace ph
{

class SdlStructFieldStump;

template<typename StructType>
class TOwnerSdlStruct : public SdlStruct
{
public:
	using OwnerType = StructType;

public:
	explicit TOwnerSdlStruct(std::string name);

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
	TBasicSdlFieldSet<TOwnedSdlField<StructType>> m_fields;
};

}// end namespace ph

#include "DataIO/SDL/Introspect/TOwnerSdlStruct.ipp"
