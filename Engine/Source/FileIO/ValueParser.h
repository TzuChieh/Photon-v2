#pragma once

#include "Common/primitive_type.h"
#include "Math/math_fwd.h"
#include "FileIO/Tokenizer.h"
#include "FileIO/FileSystem/Path.h"

#include <string>
#include <vector>

namespace ph
{

class ValueParser final
{
public:
	ValueParser(const Path& workingDirectory);

	integer               parseInteger(const std::string& integerString) const;
	real                  parseReal(const std::string& realString) const;
	std::string           parseString(const std::string& stringString) const;
	Vector3R              parseVector3r(const std::string& vector3rString) const;
	QuaternionR           parseQuaternionR(const std::string& quaternionRstring) const;
	std::vector<real>     parseRealArray(const std::string& realArrayString) const;
	std::vector<Vector3R> parseVector3rArray(const std::string& vector3rArrayString) const;

private:
	Path m_workingDirectory;

	std::string loadResource(const std::string& identifier) const;

	static bool startsWithNumber(const std::string& string);
};

}// end namespace ph