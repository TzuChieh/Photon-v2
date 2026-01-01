#pragma once

#include "Engine/Utility/utility.h"

#include <string>

namespace ph
{

/*! @brief Contains default implementations for SDL enum definitions.
Shadow the default methods in the derived class to provide actual functionality.
*/
class ISdlDefaultEnumDefinition
{
public:
	void name(std::string nameStr)
	{}

	void description(std::string desc)
	{}

	template<typename EnumType>
	void addEntry(
		EnumType         enumValue,
		std::string_view valueName,
		std::string      description = "")
	{}

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(ISdlDefaultEnumDefinition);
};

}// end namespace ph
