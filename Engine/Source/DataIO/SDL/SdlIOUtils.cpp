#include "DataIO/SDL/SdlIOUtils.h"
#include "DataIO/SDL/Tokenizer.h"
#include "DataIO/FileSystem/Path.h"

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
			throw SdlLoadError("invalid representation");
		}

		return math::Vector3R(
			parseReal(tokens[0]),
			parseReal(tokens[1]),
			parseReal(tokens[2]));
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing Vector3R -> " + e.what());
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
			throw SdlLoadError("invalid representation");
		}

		return math::QuaternionR(
			parseReal(tokens[0]),
			parseReal(tokens[1]),
			parseReal(tokens[2]),
			parseReal(tokens[3]));
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing QuaternionR -> " + e.what());
	}
}

std::vector<real> SdlIOUtils::loadRealArray(const std::string& sdlRealArrayStr)
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	try
	{
		// Tries to tokenize and see if the tokens are valid array or in fact
		// an identifier. If it is an identifier, load the actual tokens.
		//
		std::vector<std::string> realTokens;
		tokenizer.tokenize(sdlRealArrayStr, realTokens);
		if(!realTokens.empty())
		{
			if(!startsWithNumber(realTokens[0]))
			{
				const std::string& identifier = sdlRealArrayStr;
				realTokens.clear();
				tokenizer.tokenize(loadResource(identifier), realTokens);
			}
		}

		std::vector<real> realArray;
		for(const auto& realToken : realTokens)
		{
			realArray.push_back(parseReal(realToken));
		}

		return std::move(realArray);
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing real array -> " + e.what());
	}
}

std::vector<real> SdlIOUtils::loadRealArray(const Path& path)
{
	try
	{
		// Tries to tokenize and see if the tokens are valid array or in fact
		// an identifier. If it is an identifier, load the actual tokens.
		//
		std::vector<std::string> realTokens;
		tokenizer.tokenize(sdlRealArrayStr, realTokens);
		if(!realTokens.empty())
		{
			if(!startsWithNumber(realTokens[0]))
			{
				const std::string& identifier = sdlRealArrayStr;
				realTokens.clear();
				tokenizer.tokenize(loadResource(identifier), realTokens);
			}
		}

		std::vector<real> realArray;
		for(const auto& realToken : realTokens)
		{
			realArray.push_back(parseReal(realToken));
		}

		return std::move(realArray);
	}
	catch(const SdlLoadError& e)
	{
		throw SdlLoadError("on parsing real array -> " + e.what());
	}
}

bool SdlIOUtils::isResourceIdentifier(const std::string_view sdlValueStr)
{
	// Find index to the first non-blank character
	const auto pos = sdlValueStr.find_first_not_of(" \t\r\n");

	// Valid SDL resource identifier starts with a forward slash
	return pos != std::string::npos && sdlValueStr[pos] == '/';
}

}// end namespace ph
