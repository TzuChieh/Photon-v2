#pragma once

#include "FileIO/Tokenizer.h"

#include <string>

namespace ph
{

class ValueClause final
{
public:
	std::string type;
	std::string name;
	std::string value;

	ValueClause(const std::string& clauseString);
	ValueClause(const ValueClause& other);
	ValueClause(ValueClause&& other);

	bool isReference() const;
	std::string toString() const;

private:
	static const Tokenizer tokenizer;
};

}// end namespace ph