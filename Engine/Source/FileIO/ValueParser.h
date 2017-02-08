#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "FileIO/Tokenizer.h"

#include <string>
#include <vector>

namespace ph
{

class ValueParser final
{
public:
	static integer parseInteger(const std::string& integerString);
	static real parseReal(const std::string& realString);
	static std::string parseString(const std::string& stringString);
	static Vector3R parseVector3r(const std::string& vector3rString);
	static QuaternionR parseQuaternionR(const std::string& quaternionRstring);
	static std::vector<Vector3R> parseVector3rArray(const std::string& vector3rArrayString);

private:
	static const Tokenizer vector3rTokenizer;
	static const Tokenizer quaternionRtokenizer;
	static const Tokenizer vector3rArrayTokenizer;
};

}// end namespace ph