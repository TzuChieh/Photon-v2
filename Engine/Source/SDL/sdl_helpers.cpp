#include "SDL/sdl_helpers.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "SDL/Tokenizer.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/io_exceptions.h"
#include "SDL/Introspect/SdlClass.h"
#include "SDL/Introspect/SdlField.h"
#include "SDL/Introspect/SdlFunction.h"
#include "SDL/SdlOutputClause.h"
#include "SDL/sdl_exceptions.h"
#include "Utility/string_utils.h"

#include <exception>
#include <cstddef>
#include <format>

namespace ph::sdl
{

math::Vector3R load_vector3(const std::string& sdlVector3Str)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlVector3Str, tokens);

		// 3 input values form a vec3 exactly
		if(tokens.size() == 3)
		{
			return math::Vector3R(
				load_real(tokens[0]),
				load_real(tokens[1]),
				load_real(tokens[2]));
		}
		// 1 input value results in vec3 filled with the same value
		else if(tokens.size() == 1)
		{
			return math::Vector3R(load_real(tokens[0]));
		}
		else
		{
			throw SdlLoadError(
				"invalid Vector3R representation "
				"(number of values = " + std::to_string(tokens.size()) + ")");
		}
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing Vector3R -> " + e.whatStr());
	}
}

math::QuaternionR load_quaternion(const std::string& sdlQuaternionStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		// TODO: handle all equal values

		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlQuaternionStr, tokens);

		if(tokens.size() != 4)
		{
			throw SdlLoadError("invalid QuaternionR representation");
		}

		return math::QuaternionR(
			load_real(tokens[0]),
			load_real(tokens[1]),
			load_real(tokens[2]),
			load_real(tokens[3]));
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing QuaternionR -> " + e.whatStr());
	}
}

std::vector<math::Vector3R> load_vector3_array(const std::string& sdlVector3ArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'"', '"'}});

	try
	{
		std::vector<std::string> vec3Tokens;
		tokenizer.tokenize(sdlVector3ArrayStr, vec3Tokens);

		std::vector<math::Vector3R> vec3Array(vec3Tokens.size());
		for(std::size_t i = 0; i < vec3Array.size(); ++i)
		{
			vec3Array[i] = load_vector3(vec3Tokens[i]);
		}

		return vec3Array;
	}
	catch(const SdlException& e)
	{
		throw SdlLoadError("on parsing Vector3R array -> " + e.whatStr());
	}
}

void save_quaternion(const math::QuaternionR& value, std::string* const out_str)
{
	PH_ASSERT(out_str);

	try
	{
		// TODO: handle all equal values

		out_str->clear();

		std::string savedReal;

		(*out_str) += '"';
		save_real(value.x, &savedReal);
		(*out_str) += savedReal;
		(*out_str) += ' ';
		save_real(value.y, &savedReal);
		(*out_str) += savedReal;
		(*out_str) += ' ';
		save_real(value.z, &savedReal);
		(*out_str) += savedReal;
		(*out_str) += ' ';
		save_real(value.w, &savedReal);
		(*out_str) += savedReal;
		(*out_str) += '"';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving QuaternionR -> " + e.whatStr());
	}
}

void save_vector3_array(const std::vector<math::Vector3R>& values, std::string* const out_str)
{
	PH_ASSERT(out_str);

	try
	{
		(*out_str) += '{';

		std::string savedVec3;
		for(const auto& vec3 : values)
		{
			save_vector3(vec3, &savedVec3);
			(*out_str) += savedVec3;
			(*out_str) += ' ';
		}

		(*out_str) += '}';
	}
	catch(const SdlException& e)
	{
		throw SdlSaveError("on saving Vector3R array -> " + e.whatStr());
	}
}

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
