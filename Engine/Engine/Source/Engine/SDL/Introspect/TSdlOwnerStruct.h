#pragma once

#include "Engine/SDL/Introspect/SdlStruct.h"
#include "Engine/SDL/Definition/ISdlDefaultStructDefinition.h"
#include "Engine/SDL/sdl_fwd.h"
#include "Engine/SDL/Introspect/TSdlBruteForceFieldSet.h"
#include "Engine/SDL/Introspect/TSdlOwnedField.h"

#include <cstddef>
#include <string>

namespace ph
{

/*! @brief SDL binding type for a typical C++ struct.
*/
template<typename StructType>
class TSdlOwnerStruct : public SdlStruct, public ISdlDefaultStructDefinition
{
public:
	using OwnerType = StructType;

public:
	TSdlOwnerStruct();

	void initObject(
		AnyNonConstPtr         obj,
		SdlInputClauses&       clauses,
		const SdlInputContext& ctx) const override;

	void initDefaultObject(AnyNonConstPtr obj) const override;

	void saveObject(
		AnyConstPtr             obj,
		SdlOutputClauses&       clauses,
		const SdlOutputContext& ctx) const override;

	void referencedResources(
		AnyConstPtr obj,
		std::vector<const ISdlResource*>& out_resources) const override;

	/*! @brief Initialize the struct object to default values.
	*/
	void initDefaultStruct(StructType& structObj) const;

	std::size_t numFields() const override;
	const SdlField* getField(std::size_t index) const override;

	template<typename T>
	TSdlOwnerStruct& addField(T sdlField);

	template<typename StructObjType>
	TSdlOwnerStruct& addStruct(StructObjType StructType::* structObjPtr);

	template<typename StructObjType>
	TSdlOwnerStruct& addStruct(
		StructObjType StructType::* structObjPtr,
		const TSdlStructFieldStump<StructType>& structFieldStump);

	auto getFields() const -> const TSdlBruteForceFieldSet<TSdlOwnedField<StructType>>&;

	auto typeName(std::string nameStr) -> TSdlOwnerStruct&;
	TSdlOwnerStruct& description(std::string descriptionStr);

private:
	/*!
	Set fields of the struct object @p structObj to default values (as described in SDL struct definition).
	*/
	void setFieldsToDefaults(StructType& structObj) const;

	TSdlBruteForceFieldSet<TSdlOwnedField<StructType>> m_fields;
};

}// end namespace ph

#include "Engine/SDL/Introspect/TSdlOwnerStruct.ipp"
