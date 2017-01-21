#include "FileIO/ValuePacket.h"

#include <iostream>
#include <vector>

namespace ph
{

const Tokenizer ValuePacket::tokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}});

ValuePacket::ValuePacket(const std::string& valueDescription)
{
	if(valueDescription.empty())
	{
		std::cerr << "warning: at ValuePacket::ValuePacket(), input string is empty" << std::endl;
		return;
	}

	std::vector<std::string> tokens;
	tokenizer.tokenize(valueDescription, tokens);

	if(tokens.size() != 3)
	{
		std::cerr << "warning: at ValuePacket::ValuePacket(), bad number of tokens <" << tokens.size() << ">, expected to be 3" << std::endl;
		return;
	}

	type  = tokens[0];
	name  = tokens[1];
	value = tokens[2];
}

std::string ValuePacket::toString() const
{
	return "type<" + type + ">, name<" + name + ">, value<" + value + ">";
}

}// end namespace ph