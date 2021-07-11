#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/Tokenizer.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/io_exceptions.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlField.h"
#include "DataIO/SDL/Introspect/SdlFunction.h"
#include "Utility/string_utils.h"

#include <vector>

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
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing Vector3R -> " + e.whatStr());
	}
}

math::QuaternionR load_quaternion(const std::string& sdlQuaternionStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
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
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing QuaternionR -> " + e.whatStr());
	}
}

std::vector<real> load_real_array(const std::string& sdlRealArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> realTokens;
		tokenizer.tokenize(sdlRealArrayStr, realTokens);

		std::vector<real> realArray;
		for(const auto& realToken : realTokens)
		{
			realArray.push_back(load_real(realToken));
		}

		return std::move(realArray);
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing real array -> " + e.whatStr());
	}
}

std::vector<math::Vector3R> load_vector3_array(const std::string& sdlVector3ArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}});

	try
	{
		std::vector<std::string> vec3Tokens;
		tokenizer.tokenize(sdlVector3ArrayStr, vec3Tokens);

		std::vector<math::Vector3R> vec3Array;
		for(const auto& vec3Token : vec3Tokens)
		{
			vec3Array.push_back(load_vector3(vec3Token));
		}

		return std::move(vec3Array);
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing Vector3R array -> " + e.whatStr());
	}
}

bool is_resource_identifier(const std::string_view sdlValueStr)
{
	// Remove leading blank characters
	const auto trimmedStr = string_utils::trim_head(sdlValueStr);

	// Valid SDL resource identifier starts with "/"
	return !trimmedStr.empty() && trimmedStr[0] == '/';
}

bool is_reference(const std::string_view sdlValueStr)
{
	// Remove leading blank characters
	const auto trimmedStr = string_utils::trim_head(sdlValueStr);

	// Valid SDL reference starts with "@"
	return !trimmedStr.empty() && trimmedStr[0] == '@';
}

std::string gen_pretty_name(const SdlClass* const clazz)
{
	return clazz ? 
		"category: " + clazz->genCategoryName() + ", name: " + clazz->getTypeName() : 
		"unavailable";
}

std::string gen_pretty_name(const SdlFunction* const func)
{
	return func ?
		"name: " + func->getName() :
		"unavailable";
}

std::string gen_pretty_name(const SdlField* const field)
{
	return field ?
		"type: " + field->getTypeName() + ", name: " + field->getFieldName() : 
		"unavailable";
}

std::string gen_pretty_name(const SdlClass* const clazz, const SdlField* const field)
{
	return "type <" + gen_pretty_name(clazz) + ">, value <" + gen_pretty_name(field) + ">";
}

}// end namespace ph::sdl
