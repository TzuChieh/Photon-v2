#pragma once

#include <string>

namespace ph
{

/*! @brief Common SDL class definition builder.
@tparam Def Type of the class definition being built. See `ISdlDefaultClassDefinition`.
*/
template<typename Def>
class TSdlClassDefiner final
{
public:
	explicit TSdlClassDefiner(Def& def);

	/*! @brief Set the type name of the class.
	*/
	auto typeName(std::string nameStr) -> TSdlClassDefiner&;

	/*! @brief Set the name for documentation.
	*/
	auto docName(std::string nameStr) -> TSdlClassDefiner&;

	/*! @brief Set the description of the class.
	*/
	auto description(std::string desc) -> TSdlClassDefiner&;

	/*! @brief Associate another SDL class as the base of this class.
	The effect of the association depends on the actual definition.
	*/
	template<typename ClassType>
	auto baseOn() -> TSdlClassDefiner&;

	/*! @brief Register a field to the class.
	*/
	template<typename FieldType>
	auto addField(FieldType&& field) -> TSdlClassDefiner&;

	template<typename OwnerType, typename StructObjType>
	auto addStruct(StructObjType OwnerType::* structObjPtr) -> TSdlClassDefiner&;

	/*! @brief Add a function to the class.
	*/
	template<typename FunctionType>
	auto addFunction() -> TSdlClassDefiner&;

	/*! @brief Whether an instance can be created from class definition.
	*/
	auto allowCreateFromClass(bool isAllowed) -> TSdlClassDefiner&;

private:
	Def& m_def;
};

}// end namespace ph

#include "Engine/SDL/Definition/TSdlClassDefiner.ipp"
