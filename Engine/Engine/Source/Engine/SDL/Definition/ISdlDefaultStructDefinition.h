#pragma once

#include "Engine/Utility/utility.h"

#include <string>

namespace ph
{

/*! @brief Contains default implementations for SDL struct definitions.
Shadow the default methods in the derived class to provide actual functionality.
*/
class ISdlDefaultStructDefinition
{
public:
	template<typename Functor, typename FunctorCallInput>
	static auto makeDefinition()
	{}

	void typeName(std::string nameStr)
	{}

	void description(std::string desc)
	{}

	template<typename FieldType>
	void addField(FieldType field)
	{}

	template<typename OwnerType, typename StructObjType>
	void addStruct(StructObjType OwnerType::* structObjPtr)
	{}

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(ISdlDefaultStructDefinition);
};

}// end namespace ph
