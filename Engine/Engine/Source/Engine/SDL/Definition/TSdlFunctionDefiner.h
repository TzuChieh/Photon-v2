#pragma once

#include <string>

namespace ph
{

/*! @brief Common SDL function definition builder.
@tparam Def Type of the function definition being built. See `ISdlDefaultFunctionDefinition`.
*/
template<typename Def>
class TSdlFunctionDefiner final
{
public:
	explicit TSdlFunctionDefiner(Def& def);

	/*! @brief Set the name of the function.
	*/
	auto name(std::string nameStr) -> TSdlFunctionDefiner&;

	/*! @brief Set the description of the function.
	*/
	auto description(std::string desc) -> TSdlFunctionDefiner&;

	/*! @brief Register a parameter to the function.
	*/
	template<typename T>
	auto addParam(T&& param) -> TSdlFunctionDefiner&;

private:
	Def& m_def;
};

}// end namespace ph

#include "Engine/SDL/Definition/TSdlFunctionDefiner.ipp"
