#include "FileIO/ValueParser.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "FileIO/TextFileLoader.h"
#include "FileIO/SDL/SdlResourceIdentifier.h"

#include <iostream>
#include <string>
#include <cctype>

namespace ph
{

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

Vector3R ValueParser::parseVector3r(const std::string& vector3rString) const
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	std::vector<std::string> tokens;
	tokenizer.tokenize(vector3rString, tokens);

	if(tokens.size() != 3)
	{
		std::cerr << "warning: at ValueParser::parseVector3r(), bad string representation <" + vector3rString + ">, " <<
		             Vector3R().toString() << " is returned "<< std::endl;
		return Vector3R();
	}

	return Vector3R(static_cast<real>(std::stold(tokens[0])), 
	                static_cast<real>(std::stold(tokens[1])), 
	                static_cast<real>(std::stold(tokens[2])));
}

QuaternionR ValueParser::parseQuaternionR(const std::string& quaternionRstring) const
{
	static const Tokenizer tokenizer({ ' ', '\t', '\n', '\r' }, {});

	std::vector<std::string> tokens;
	tokenizer.tokenize(quaternionRstring, tokens);

	if(tokens.size() != 4)
	{
		std::cerr << "warning: at ValueParser::parseQuaternionR(), bad string representation <" + quaternionRstring + ">, " <<
		             QuaternionR().toString() << " is returned " << std::endl;
		return QuaternionR();
	}

	return QuaternionR(static_cast<real>(std::stold(tokens[0])),
	                   static_cast<real>(std::stold(tokens[1])),
	                   static_cast<real>(std::stold(tokens[2])), 
	                   static_cast<real>(std::stold(tokens[3])));
}

std::vector<real> ValueParser::parseRealArray(const std::string& realArrayString) const
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {});

	// Tries to tokenize and see if the tokens are valid array or in fact
	// an identifier. If its an identifier, load the actual tokens.
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

		std::cerr << realValue << std::endl;
	}

	return std::move(realArray);
}

std::vector<Vector3R> ValueParser::parseVector3rArray(const std::string& vector3rArrayString) const
{
	static const Tokenizer tokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}});

	std::vector<std::string> tokens;
	tokenizer.tokenize(vector3rArrayString, tokens);

	std::vector<Vector3R> results;
	for(const auto& token : tokens)
	{
		results.push_back(parseVector3r(token));
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