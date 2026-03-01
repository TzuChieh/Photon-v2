#pragma once

#include "Engine/SDL/Introspect/SdlStruct.h"
#include "Engine/SDL/Definition/ISdlDefaultStructDefinition.h"
#include "Engine/SDL/sdl_fwd.h"
#include "Engine/SDL/Introspect/FieldSet/TSdlDefaultFieldSet.h"
#include "Engine/SDL/Introspect/TSdlOwnedField.h"

#include <cstddef>
#include <string>

namespace ph
{

/*! @brief SDL binding type for a typical C++ struct.
*/
template<typename StructType, typename FieldSet = TSdlDefaultFieldSet<TSdlOwnedField<StructType>>>
class TSdlOwnerStruct : public SdlStruct, public ISdlDefaultStructDefinition
{
public:
	using OwnerType = StructType;

public:
	TSdlOwnerStruct();

	void initObject(
		const SdlNonConstInstance& obj,
		SdlInputClauses&           clauses,
		const SdlInputContext&     ctx) const override;

	void initDefaultObject(const SdlNonConstInstance& obj) const override;

	void saveObject(
		const SdlConstInstance&    obj,
		SdlOutputClauses&          clauses,
		const SdlOutputContext&    ctx) const override;

	void referencedResources(
		const SdlConstInstance& obj,
		std::vector<const ISdlResource*>& out_resources) const override;

	/*! @brief Initialize the struct object to default values.
	*/
	void initDefaultStruct(StructType& structObj) const;

	SdlInstantiated instantiate() const override;
	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;

	template<typename T>
	auto addField(T sdlField) -> TSdlOwnerStruct&;

	template<typename StructObjType>
	auto addStruct(StructObjType StructType::* structObjPtr) -> TSdlOwnerStruct&;

	template<typename StructObjType>
	auto addStruct(
		StructObjType StructType::* structObjPtr,
		const TSdlStructFieldStub<StructType>& structFieldStub) -> TSdlOwnerStruct&;

	auto getFields() const -> const FieldSet&;

	auto typeName(std::string nameStr) -> TSdlOwnerStruct&;
	auto description(std::string descriptionStr) -> TSdlOwnerStruct&;
	auto userSpec(SdlUserSpec spec) -> TSdlOwnerStruct&;

private:
	/*!
	Set fields of the struct object @p structObj to default values (as described in SDL struct definition).
	*/
	void setFieldsToDefaults(StructType& structObj) const;

	FieldSet m_fields;
};

}// end namespace ph

#include "Engine/SDL/Introspect/TSdlOwnerStruct.ipp"
