#pragma once

#include "Engine/Utility/utility.h"

#include <string>

namespace ph
{

/*! @brief Contains default implementations for SDL class definitions.
Shadow the default methods in the derived class to provide actual functionality.
*/
class ISdlDefaultClassDefinition
{
public:
	void typeName(std::string nameStr)
	{}

	void docName(std::string nameStr)
	{}

	void description(std::string desc)
	{}

	template<typename ClassType>
	void baseOn()
	{}

	template<typename FieldType>
	void addField(FieldType field)
	{}

	template<typename OwnerType, typename StructObjType>
	void addStruct(StructObjType OwnerType::* structObjPtr)
	{}

	template<typename FunctionType>
	void addFunction()
	{}

	void allowCreateFromClass(bool isAllowed)
	{}

// Hide special members as this class is not intended to be used polymorphically.
// It is derived class's choice to expose them (by defining them in public) or not.
protected:
	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(ISdlDefaultClassDefinition);
};

}// end namespace ph
