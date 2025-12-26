#pragma once

#include <string>

namespace ph
{

/*! @brief Common SDL struct definition builder.
@tparam Def Type of the struct definition being built. See `ISdlDefaultStructDefinition`.
*/
template<typename Def>
class TSdlStructDefiner final
{
public:
	explicit TSdlStructDefiner(Def& def);

	/*! @brief Set the type name of the struct.
	*/
	auto typeName(std::string nameStr) -> TSdlStructDefiner&;

	/*! @brief Set the description of the struct.
	*/
	auto description(std::string desc) -> TSdlStructDefiner&;

	/*! @brief Register a field to the struct.
	*/
	template<typename FieldType>
	auto addField(FieldType&& field) -> TSdlStructDefiner&;

	template<typename OwnerType, typename StructObjType>
	auto addStruct(StructObjType OwnerType::* structObjPtr) -> TSdlStructDefiner&;

private:
	Def& m_def;
};

}// end namespace ph

#include "Engine/SDL/Definition/TSdlStructDefiner.ipp"
