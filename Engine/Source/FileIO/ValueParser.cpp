#include "FileIO/ValueParser.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"

#include <iostream>

namespace ph
{

const Tokenizer ValueParser::vector3rTokenizer({' ', '\t', '\n', '\r'}, {});
const Tokenizer ValueParser::quaternionRtokenizer({' ', '\t', '\n', '\r'}, {});
const Tokenizer ValueParser::vector3rArrayTokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}});

integer ValueParser::parseInteger(const std::string& integerString)
{
	return static_cast<integer>(std::stoll(integerString));
}

real ValueParser::parseReal(const std::string& realString)
{
	return static_cast<real>(std::stold(realString));
}


std::string ValueParser::parseString(const std::string& stringString)
{
	return stringString;
}

Vector3R ValueParser::parseVector3r(const std::string& vector3rString)
{
	std::vector<std::string> tokens;
	vector3rTokenizer.tokenize(vector3rString, tokens);

	if(tokens.size() != 3)
	{
		std::cerr << "warning: at ValueParser::parseVector3r(), bad string representation <" + vector3rString + ">, " <<
		             Vector3R().toStringFormal() << " is returned "<< std::endl;
		return Vector3R();
	}

	return Vector3R(static_cast<real>(std::stold(tokens[0])), 
	                static_cast<real>(std::stold(tokens[1])), 
	                static_cast<real>(std::stold(tokens[2])));
}

QuaternionR ValueParser::parseQuaternionR(const std::string& quaternionRstring)
{
	std::vector<std::string> tokens;
	quaternionRtokenizer.tokenize(quaternionRstring, tokens);

	if(tokens.size() != 4)
	{
		std::cerr << "warning: at ValueParser::parseQuaternionR(), bad string representation <" + quaternionRstring + ">, " <<
		             QuaternionR().toStringFormal() << " is returned " << std::endl;
		return QuaternionR();
	}

	return QuaternionR(static_cast<real>(std::stold(tokens[0])),
	                   static_cast<real>(std::stold(tokens[1])),
	                   static_cast<real>(std::stold(tokens[2])), 
	                   static_cast<real>(std::stold(tokens[3])));
}

std::vector<Vector3R> ValueParser::parseVector3rArray(const std::string& vector3rArrayString)
{
	std::vector<std::string> tokens;
	vector3rArrayTokenizer.tokenize(vector3rArrayString, tokens);

	std::vector<Vector3R> results;
	for(const auto& token : tokens)
	{
		results.push_back(parseVector3r(token));
	}

	return results;
}

}// end namespace ph