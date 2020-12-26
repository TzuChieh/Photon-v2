#pragma once

#include "DataIO/SDL/Tokenizer.h"

#include <string>

namespace ph
{

class ValueClause final
{
public:
	std::string type;
	std::string name;
	std::string value;

	explicit ValueClause(const std::string& clauseString);
	ValueClause(const ValueClause& other);
	ValueClause(ValueClause&& other);

	bool isReference() const;
	std::string toString() const;
	std::string genPrettyName() const;

private:
	static const Tokenizer tokenizer;
};

// In-header Implementations:

inline std::string ValueClause::genPrettyName() const
{
	return "type: " + type + ", name: " + name;
}

}// end namespace ph
