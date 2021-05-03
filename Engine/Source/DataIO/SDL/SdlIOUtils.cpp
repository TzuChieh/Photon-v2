#include "DataIO/SDL/SdlIOUtils.h"
#include "DataIO/SDL/Tokenizer.h"
#include "DataIO/FileSystem/Path.h"
#include "DataIO/IOUtils.h"
#include "DataIO/io_exceptions.h"
#include "DataIO/SDL/Introspect/SdlClass.h"
#include "DataIO/SDL/Introspect/SdlField.h"

namespace ph
{

real SdlIOUtils::loadReal(const std::string& sdlRealStr)
{
	return parseReal(sdlRealStr);
}

integer SdlIOUtils::loadInteger(const std::string& sdlIntegerStr)
{
	return parseInteger(sdlIntegerStr);
}

math::Vector3R SdlIOUtils::loadVector3R(const std::string& sdlVector3Str)
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
			parseReal(tokens[0]),
			parseReal(tokens[1]),
			parseReal(tokens[2]));
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing Vector3R -> " + e.whatStr());
	}
}

math::QuaternionR SdlIOUtils::loadQuaternionR(const std::string& sdlQuaternionStr)
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
			parseReal(tokens[0]),
			parseReal(tokens[1]),
			parseReal(tokens[2]),
			parseReal(tokens[3]));
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing QuaternionR -> " + e.whatStr());
	}
}

std::vector<real> SdlIOUtils::loadRealArray(const std::string& sdlRealArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		std::vector<std::string> realTokens;
		tokenizer.tokenize(sdlRealArrayStr, realTokens);

		std::vector<real> realArray;
		for(const auto& realToken : realTokens)
		{
			realArray.push_back(parseReal(realToken));
		}

		return std::move(realArray);
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing real array -> " + e.whatStr());
	}
}

std::vector<real> SdlIOUtils::loadRealArray(const Path& filePath)
{
	try
	{
		return loadRealArray(IOUtils::loadText(filePath));
	}
	catch(const FileIOError& e)
	{
		throw SdlLoadError("on loading real array -> " + e.whatStr());
	}
}

std::vector<math::Vector3R> SdlIOUtils::loadVector3RArray(const std::string& sdlVector3ArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}});

	try
	{
		std::vector<std::string> vec3Tokens;
		tokenizer.tokenize(sdlVector3ArrayStr, vec3Tokens);

		std::vector<math::Vector3R> vec3Array;
		for(const auto& vec3Token : vec3Tokens)
		{
			vec3Array.push_back(loadVector3R(vec3Token));
		}

		return std::move(vec3Array);
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing Vector3R array -> " + e.whatStr());
	}
}

bool SdlIOUtils::isResourceIdentifier(const std::string_view sdlValueStr)
{
	// Find index to the first non-blank character
	const auto pos = sdlValueStr.find_first_not_of(" \t\r\n");

	// Valid SDL resource identifier starts with "/"
	return pos != std::string::npos && sdlValueStr[pos] == '/';
}

bool SdlIOUtils::isReference(const std::string_view sdlValueStr)
{
	// Find index to the first non-blank character
	const auto pos = sdlValueStr.find_first_not_of(" \t\r\n");

	// Valid SDL reference starts with "@"
	return pos != std::string::npos && sdlValueStr[pos] == '@';
}

std::string SdlIOUtils::genPrettyName(const SdlClass* const clazz)
{
	return clazz ? 
		"category: " + clazz->getCategory() + ", name: " + clazz->getTypeName() : 
		"unavailable";
}

std::string SdlIOUtils::genPrettyName(const SdlField* const field)
{
	return field ?
		"type: " + field->getTypeName() + ", name: " + field->getFieldName() : 
		"unavailable";
}

std::string SdlIOUtils::genPrettyName(const SdlClass* const clazz, const SdlField* const field)
{
	return "type <" + genPrettyName(clazz) + ">, value <" + genPrettyName(field) + ">";
}

}// end namespace ph
