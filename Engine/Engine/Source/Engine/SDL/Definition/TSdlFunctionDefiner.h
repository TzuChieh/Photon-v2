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

	template<typename T>
	auto addParam(T&& param) -> TSdlFunctionDefiner&;

	auto description(std::string desc) -> TSdlFunctionDefiner&;

private:
	Def& m_def;
};

}// end namespace ph

#include "Engine/SDL/Definition/TSdlFunctionDefiner.ipp"
// #pragma once