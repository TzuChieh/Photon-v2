#pragma once

#include "SDL/Introspect/SdlStruct.h"
#include "SDL/sdl_fwd.h"
#include "SDL/Introspect/TSdlBruteForceFieldSet.h"
#include "SDL/Introspect/TSdlOwnedField.h"

#include <cstddef>
#include <string>

namespace ph
{

/*! @brief SDL binding type for a typical C++ struct.
*/
template<typename StructType>
class TSdlOwnerStruct : public SdlStruct
{
public:
	using OwnerType = StructType;

public:
	explicit TSdlOwnerStruct(std::string name);

	/*! @brief Initialize a struct object to default values.
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
		const SdlStructFieldStump&  structFieldStump);

	auto getFields() const -> const TSdlBruteForceFieldSet<TSdlOwnedField<StructType>>&;

	TSdlOwnerStruct& description(std::string descriptionStr);

private:
	/*!
	Set fields of the struct object @p structObj to default values (as described in SDL struct definition).
	*/
	void setFieldsToDefaults(StructType& structObj) const;

	TSdlBruteForceFieldSet<TSdlOwnedField<StructType>> m_fields;
};

}// end namespace ph

#include "SDL/Introspect/TSdlOwnerStruct.ipp"
