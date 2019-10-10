#include "FileIO/SDL/ValueClause.h"

#include <iostream>
#include <vector>

namespace ph
{

const Tokenizer ValueClause::tokenizer({' ', '\t', '\n', '\r'}, {{'\"', '\"'}, {'{', '}'}});

ValueClause::ValueClause(const std::string& clauseString)
{
	if(clauseString.empty())
	{
		std::cerr << "warning: at ValueClause::ValueClause(), input string is empty" << std::endl;
		return;
	}

	std::vector<std::string> tokens;
	tokenizer.tokenize(clauseString, tokens);

	if(tokens.size() != 3)
	{
		std::cerr << "warning: at ValueClause::ValueClause(), bad number of tokens <" << tokens.size() << 
                     ">, expected to be 3" << std::endl;
		return;
	}

	type  = tokens[0];
	name  = tokens[1];
	value = tokens[2];
}

ValueClause::ValueClause(const ValueClause& other) : 
	type(other.type), name(other.name), value(other.value)
{}

ValueClause::ValueClause(ValueClause&& other) : 
	type(std::move(other.type)), name(std::move(other.name)), value(std::move(other.value))
{}

bool ValueClause::isReference() const
{
	return value.empty() ? false : value[0] == '@';
}

std::string ValueClause::toString() const
{
	return "type<" + type + ">, name<" + name + ">, value<" + value + ">";
}

}// end namespace ph
