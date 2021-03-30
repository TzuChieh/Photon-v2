#include "DataIO/SDL/ValueParser.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "DataIO/TextFileLoader.h"
#include "DataIO/SDL/SdlResourceIdentifier.h"
#include "DataIO/SDL/Tokenizer.h"

#include <iostream>
#include <string>
#include <cctype>

namespace ph
{

Logger ValueParser::logger = Logger(LogSender("Value Parser"));

ValueParser::ValueParser(const Path& workingDirectory) : 
	m_workingDirectory(workingDirectory)
{}

integer ValueParser::parseInteger(const std::string& integerString) const
{
	return static_cast<integer>(std::stoll(integerString));
}

real ValueParser::parseReal(const std::string& realString) const
{
	return static_cast<real>(std::stold(realString));
}

std::string ValueParser::parseString(const std::string& stringString) const
{
	return stringString;
}

math::Vector3R ValueParser::parseVector3(const std::string& vector3String) const
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	std::vector<std::string> tokens;
	tokenizer.tokenize(vector3String, tokens);

	if(tokens.size() != 3)
	{
		std::cerr << "warning: at ValueParser::parseVector3(), bad string representation <" + vector3String + ">, " <<
		             math::Vector3R(0).toString() << " is returned "<< std::endl;
		return math::Vector3R(0);
	}

	return math::Vector3R(
		static_cast<real>(std::stold(tokens[0])),
		static_cast<real>(std::stold(tokens[1])), 
		static_cast<real>(std::stold(tokens[2])));
}

math::QuaternionR ValueParser::parseQuaternion(const std::string& quaternionString) const
{
	static const Tokenizer tokenizer({ ' ', '\t', '\n', '\r' }, {});

	std::vector<std::string> tokens;
	tokenizer.tokenize(quaternionString, tokens);

	if(tokens.size() != 4)
	{
		std::cerr << "warning: at ValueParser::parseQuaternion(), bad string representation <" + quaternionString + ">, " <<
		             math::QuaternionR::makeNoRotation().toString() << " is returned " << std::endl;
		return math::QuaternionR::makeNoRotation();
	}

	return math::QuaternionR(
		static_cast<real>(std::stold(tokens[0])),
		static_cast<real>(std::stold(tokens[1])),
		static_cast<real>(std::stold(tokens[2])), 
		static_cast<real>(std::stold(tokens[3])));
}

std::vector<real> ValueParser::parseRealArray(const std::string& realArrayString) const
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	// Tries to tokenize and see if the tokens are valid array or in fact
	// an identifier. If it is an identifier, load the actual tokens.
	//
	std::vector<std::string> realTokens;
	tokenizer.tokenize(realArrayString, realTokens);
	if(!realTokens.empty())
	{
		if(!startsWithNumber(realTokens[0]))
		{
			const std::string& identifier = realArrayString;
			realTokens.clear();
			tokenizer.tokenize(loadResource(identifier), realTokens);
		}
	}

	std::vector<real> realArray;
	for(const auto& realToken : realTokens)
	{
		const long double realValue = std::stold(realToken);
		realArray.push_back(static_cast<real>(realValue));
	}

	return std::move(realArray);
}

std::vector<math::Vector3R> ValueParser::parseVector3Array(const std::string& vector3ArrayString) const
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}});

	std::vector<std::string> tokens;
	tokenizer.tokenize(vector3ArrayString, tokens);

	std::vector<math::Vector3R> results;
	for(const auto& token : tokens)
	{
		results.push_back(parseVector3(token));
	}

	return results;
}

std::string ValueParser::loadResource(const std::string& identifier) const
{
	const SdlResourceIdentifier sdlri(identifier, m_workingDirectory);

	std::string resource;
	if(!TextFileLoader::load(sdlri.getPathToResource(), &resource))
	{
		std::cerr << "warning: at ValueParser::loadResource(), "
		          << "specified SDL resource identifier <"
		          << identifier
		          << "> cannot be loaded" << std::endl;
	}

	return std::move(resource);
}

bool ValueParser::startsWithNumber(const std::string& string)
{
	if(string.empty())
	{
		return false;
	}

	return std::isdigit(string[0]);
}

}// end namespace ph
