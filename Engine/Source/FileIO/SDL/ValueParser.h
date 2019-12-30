#pragma once

#include "Common/primitive_type.h"
#include "Common/Logger.h"
#include "Math/math_fwd.h"
#include "FileIO/FileSystem/Path.h"

#include <string>
#include <vector>

namespace ph
{

class ValueParser final
{
public:
	explicit ValueParser(const Path& workingDirectory);

	integer parseInteger(const std::string& integerString) const;
	real parseReal(const std::string& realString) const;
	std::string parseString(const std::string& stringString) const;
	math::Vector3R parseVector3(const std::string& vector3String) const;
	math::QuaternionR parseQuaternion(const std::string& quaternionstring) const;
	std::vector<real> parseRealArray(const std::string& realArrayString) const;
	std::vector<math::Vector3R> parseVector3Array(const std::string& vector3ArrayString) const;

private:
	std::string loadResource(const std::string& identifier) const;

	static bool startsWithNumber(const std::string& string);

	Path m_workingDirectory;

	static Logger logger;
};

}// end namespace ph
