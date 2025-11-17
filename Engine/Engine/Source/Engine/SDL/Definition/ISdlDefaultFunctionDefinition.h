#pragma once

#include "Engine/Utility/utility.h"

#include <string>

namespace ph
{

/*! @brief Contains default implementations for SDL function definitions.
Shadow the default methods in the derived class to provide actual functionality.
*/
class ISdlDefaultFunctionDefinition
{
public:
	template<typename Functor, typename FunctorCallInput>
	static auto makeDefinition()
	{}

	template<typename T>
	void addParam(T param)
	{}

	void description(std::string desc)
	{}

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(ISdlDefaultFunctionDefinition);
};

}// end namespace ph
