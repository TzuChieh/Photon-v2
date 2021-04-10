#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "DataIO/SDL/sdl_exceptions.h"

#include <vector>
#include <string>
#include <string_view>
#include <exception>

namespace ph
{

class Path;

class SdlIOUtils final
{
public:
	static real loadReal(const std::string& sdlRealStr);
	static integer loadInteger(const std::string& sdlIntegerStr);
	static math::Vector3R loadVector3R(const std::string& sdlVector3Str);
	static math::QuaternionR loadQuaternionR(const std::string& sdlQuaternionStr);
	static std::vector<real> loadRealArray(const std::string& sdlRealArrayStr);
	static std::vector<real> loadRealArray(const Path& filePath);

	/*! @brief Check whether the string represents a SDL resource identifier.

	Checks the format of the string only. Does not test whether the identifier
	actually points to a valid resource or not.
	*/
	static bool isResourceIdentifier(std::string_view sdlValueStr);

private:
	static real parseReal(const std::string& sdlRealStr);
	static integer parseInteger(const std::string& sdlIntegerStr);
};

// In-header Implementations:

inline real SdlIOUtils::parseReal(const std::string& sdlRealStr)
{
	try
	{
		// TODO: check for overflow after cast?
		return static_cast<real>(std::stold(sdlRealStr));
	}
	catch(const std::invalid_argument& e)
	{
		throw SdlLoadError("invalid real representation -> " + std::string(e.what()));
	}
	catch(const std::out_of_range& e)
	{
		throw SdlLoadError("parsed real overflow -> " + std::string(e.what()));
	}
}

inline integer SdlIOUtils::parseInteger(const std::string& sdlIntegerStr)
{
	try
	{
		// TODO: check for overflow after cast?
		return static_cast<integer>(std::stoll(sdlIntegerStr));
	}
	catch(const std::invalid_argument& e)
	{
		throw SdlLoadError("invalid integer representation -> " + std::string(e.what()));
	}
	catch(const std::out_of_range& e)
	{
		throw SdlLoadError("parsed integer overflow -> " + std::string(e.what()));
	}
}

}// end namespace ph
