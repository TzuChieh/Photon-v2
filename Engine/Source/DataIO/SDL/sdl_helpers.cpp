#include "DataIO/SDL/sdl_helpers.h"
#include "DataIO/SDL/Tokenizer.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/io_exceptions.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlField.h"
#include "DataIO/SDL/Introspect/SdlFunction.h"

#include <unordered_map>

namespace ph::sdl
{

math::Vector3R load_vector3(const std::string& sdlVector3Str)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> tokens;
		tokenizer.tokenize(sdlVector3Str, tokens);

		if(tokens.size() != 3)
		{
			throw SdlLoadError("invalid Vector3R representation");
		}

		return math::Vector3R(
			detail::parse_real(tokens[0]),
			detail::parse_real(tokens[1]),
			detail::parse_real(tokens[2]));
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
			detail::parse_real(tokens[0]),
			detail::parse_real(tokens[1]),
			detail::parse_real(tokens[2]),
			detail::parse_real(tokens[3]));
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
			realArray.push_back(detail::parse_real(realToken));
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
	// Find index to the first non-blank character
	const auto pos = sdlValueStr.find_first_not_of(" \t\r\n");

	// Valid SDL resource identifier starts with "/"
	return pos != std::string::npos && sdlValueStr[pos] == '/';
}

bool is_reference(const std::string_view sdlValueStr)
{
	// Find index to the first non-blank character
	const auto pos = sdlValueStr.find_first_not_of(" \t\r\n");

	// Valid SDL reference starts with "@"
	return pos != std::string::npos && sdlValueStr[pos] == '@';
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

std::string category_to_string(const ETypeCategory category)
{
	std::string categoryName;
	switch(category)
	{
	case ETypeCategory::REF_GEOMETRY:         categoryName = "geometry";         break;
	case ETypeCategory::REF_MOTION:           categoryName = "motion";           break;
	case ETypeCategory::REF_MATERIAL:         categoryName = "material";         break;
	case ETypeCategory::REF_LIGHT_SOURCE:     categoryName = "light-source";     break;
	case ETypeCategory::REF_ACTOR:            categoryName = "actor";            break;
	case ETypeCategory::REF_FRAME_PROCESSOR:  categoryName = "frame-processor";  break;
	case ETypeCategory::REF_IMAGE:            categoryName = "image";            break;
	case ETypeCategory::REF_RECEIVER:         categoryName = "receiver";         break;
	case ETypeCategory::REF_SAMPLE_GENERATOR: categoryName = "sample-generator"; break;
	case ETypeCategory::REF_RENDERER:         categoryName = "renderer";         break;
	case ETypeCategory::REF_OPTION:           categoryName = "option";           break;
	case ETypeCategory::UNSPECIFIED:          categoryName = "unspecified";      break;

	default:
		// All categories must already have a string equivalent entry
		PH_ASSERT_UNREACHABLE_SECTION();
		break;
	}

	return categoryName;
}

ETypeCategory string_to_category(const std::string& categoryStr)
{
	const static std::unordered_map<std::string, ETypeCategory> map = 
	{ 
		{category_to_string(ETypeCategory::REF_GEOMETRY),         ETypeCategory::REF_GEOMETRY},
		{category_to_string(ETypeCategory::REF_MATERIAL),         ETypeCategory::REF_MATERIAL},
		{category_to_string(ETypeCategory::REF_MOTION),           ETypeCategory::REF_MOTION},
		{category_to_string(ETypeCategory::REF_LIGHT_SOURCE),     ETypeCategory::REF_LIGHT_SOURCE},
		{category_to_string(ETypeCategory::REF_ACTOR),            ETypeCategory::REF_ACTOR},
		{category_to_string(ETypeCategory::REF_RECEIVER),         ETypeCategory::REF_RECEIVER},
		{category_to_string(ETypeCategory::REF_IMAGE),            ETypeCategory::REF_IMAGE},
		{category_to_string(ETypeCategory::REF_SAMPLE_GENERATOR), ETypeCategory::REF_SAMPLE_GENERATOR},
		{category_to_string(ETypeCategory::REF_FRAME_PROCESSOR),  ETypeCategory::REF_FRAME_PROCESSOR},
		{category_to_string(ETypeCategory::REF_RENDERER),         ETypeCategory::REF_RENDERER},
		{category_to_string(ETypeCategory::REF_OPTION),           ETypeCategory::REF_OPTION}

		// Any other strings including the string for ETypeCategory::UNSPECIFIED
		// is not explicitly mapped here, as by default they all mapped to ETypeCategory::UNSPECIFIED
	};

	const auto& iter = map.find(categoryStr);
	if(iter == map.end())
	{
		// Map to "unspecified" by default
		return ETypeCategory::UNSPECIFIED;
	}

	return iter->second;
}

}// end namespace ph::sdl
