#include "SDL/sdl_helpers.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/Introspect/SdlStruct.h"
#include "SDL/Introspect/SdlField.h"
#include "SDL/Introspect/SdlFunction.h"
#include "SDL/SdlOutputClause.h"

#include <Common/assertion.h>
#include <Common/Utility/string_utils.h>

#include <format>

namespace ph::sdl
{

void save_field_id(const SdlField* const sdlField, SdlOutputClause& clause)
{
	PH_ASSERT(sdlField);

	clause.type = sdlField->getTypeName();
	clause.name = sdlField->getFieldName();
}

std::string gen_pretty_name(const SdlClass* const clazz)
{
	return clazz
		? std::format("category: {}, name: {}", clazz->genCategoryName(), clazz->getTypeName())
		: "unavailable";
}

std::string gen_pretty_name(const SdlStruct* const ztruct)
{
	return ztruct
		? std::format("name: {}", ztruct->getTypeName())
		: "unavailable";
}

std::string gen_pretty_name(const SdlFunction* const func)
{
	return func
		? std::format("name: {}", func->getName())
		: "unavailable";
}

std::string gen_pretty_name(const SdlField* const field)
{
	return field
		? std::format("type: {}, name: {}", field->getTypeName(), field->getFieldName())
		: "unavailable";
}

std::string gen_pretty_name(const SdlClass* const clazz, const SdlField* const field)
{
	return std::format(
		"SDL class <{}>, value <{}>",
		gen_pretty_name(clazz), gen_pretty_name(field));
}

auto get_all_callable_functions(const SdlClass* const callableParentClass)
-> std::vector<std::pair<const SdlFunction*, const SdlClass*>>
{
	std::vector<std::pair<const SdlFunction*, const SdlClass*>> results;
	
	const SdlClass* currentClass = callableParentClass;
	while(currentClass)
	{
		for(std::size_t fi = 0; fi < currentClass->numFunctions(); ++fi)
		{
			results.push_back({currentClass->getFunction(fi), currentClass});
		}

		currentClass = currentClass->getBase();
	}

	return results;
}

std::string name_to_title_case(const std::string_view sdlName)
{
	// The implementation is aware of empty inputs

	auto result = std::string(sdlName);

	// Capitalize first character if possible
	if(!result.empty() && result.front() != '-')
	{
		result.front() = string_utils::az_to_AZ(result.front());
	}

	for(std::size_t i = 1; i < result.size(); ++i)
	{
		const char ch = result[i];

		// Find all dashes and make it a space
		if(ch == '-')
		{
			result[i] = ' ';

			// Capitalize the character after space if possible
			if(i + 1 < result.size())
			{
				result[i + 1] = string_utils::az_to_AZ(result[i + 1]);
			}
		}
	}

	return result;
}

std::string name_to_camel_case(const std::string_view sdlName, const bool capitalizedFront)
{
	// The implementation is aware of empty inputs

	std::string camelCase = name_to_title_case(sdlName);
	string_utils::erase_all(camelCase, ' ');

	if(!capitalizedFront && !camelCase.empty())
	{
		camelCase.front() = string_utils::AZ_to_az(camelCase.front());
	}

	return camelCase;
}

std::string name_to_snake_case(const std::string_view sdlName)
{
	// The implementation is aware of empty inputs

	std::string snakeCase = name_to_title_case(sdlName);

	for(std::size_t i = 0; i < snakeCase.size(); ++i)
	{
		// Replace each space in title with dash
		if(snakeCase[i] == ' ')
		{
			snakeCase[i] = '_';
		}

		// Make A~Z characters lower-case
		snakeCase[i] = string_utils::AZ_to_az(snakeCase[i]);
	}

	return snakeCase;
}

}// end namespace ph::sdl
