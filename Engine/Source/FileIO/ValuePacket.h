#pragma once

#include "FileIO/Tokenizer.h"

#include <string>

namespace ph
{

class ValuePacket final
{
public:
	std::string type;
	std::string name;
	std::string value;

	ValuePacket(const std::string& valueDescription);

	std::string toString() const;

private:
	static const Tokenizer tokenizer;
};

}// end namespace ph