#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "FileIO/Tokenizer.h"

#include <string>

namespace ph
{

class ValueParser final
{
public:
	static integer parseInteger(const std::string& integerString);
	static real parseReal(const std::string& realString);
	static std::string parseString(const std::string& stringString);
	static Vector3R parseVector3r(const std::string& vector3rString);

private:
	static const Tokenizer vector3rTokenizer;
};

}// end namespace ph