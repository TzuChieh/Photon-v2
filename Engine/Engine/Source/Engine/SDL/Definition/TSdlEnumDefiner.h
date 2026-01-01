#pragma once

#include <string>

namespace ph
{

/*! @brief Common SDL enum definition builder.
@tparam Def Type of the enum definition being built. See `ISdlDefaultEnumDefinition`.
*/
template<typename Def>
class TSdlEnumDefiner final
{
public:
	explicit TSdlEnumDefiner(Def& def);

	/*! @brief Set the name of the enum.
	*/
	auto name(std::string nameStr) -> TSdlEnumDefiner&;

	/*! @brief Set the description of the enum.
	*/
	auto description(std::string desc) -> TSdlEnumDefiner&;

	/*! @brief Register an entry (enumeration) to the enum.
	*/
	template<typename EnumType>
	auto addEntry(
		EnumType         enumValue,
		std::string_view valueName,
		std::string      description = "") -> TSdlEnumDefiner&;

private:
	Def& m_def;
};

}// end namespace ph

#include "Engine/SDL/Definition/TSdlEnumDefiner.ipp"
