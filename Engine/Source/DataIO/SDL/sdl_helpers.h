#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/ETypeCategory.h"

#include <vector>
#include <string>
#include <string_view>
#include <exception>

namespace ph
{

class SdlClass;
class SdlField;
class SdlFunction;

}// end namespace ph

namespace ph::sdl
{

// TODO: templatize vec3, quat related funcs

real load_real(const std::string& sdlRealStr);
integer load_integer(const std::string& sdlIntegerStr);
math::Vector3R load_vector3(const std::string& sdlVector3Str);
math::QuaternionR load_quaternion(const std::string& sdlQuaternionStr);
std::vector<real> load_real_array(const std::string& sdlRealArrayStr);
std::vector<math::Vector3R> load_vector3_array(const std::string& sdlVector3ArrayStr);

/*! @brief Check whether the string represents a SDL resource identifier.

Checks the format of the string only. Does not test whether the identifier
actually points to a valid resource or not.
*/
bool is_resource_identifier(std::string_view sdlValueStr);

/*! @brief Check whether the string represents a SDL reference.

Checks the format of the string only. Does not test whether the reference
actually points to a valid resource or not.
*/
bool is_reference(std::string_view sdlValueStr);

std::string gen_pretty_name(const SdlClass* clazz);
std::string gen_pretty_name(const SdlField* field);
std::string gen_pretty_name(const SdlFunction* func);
std::string gen_pretty_name(const SdlClass* clazz, const SdlField* field);

std::string category_to_string(ETypeCategory category);
ETypeCategory string_to_category(const std::string& categoryStr);

namespace detail
{

/*! @brief Returns a real by processing its SDL representation.
*/
real parse_real(const std::string& sdlRealStr);

/*! @brief Returns an integer by processing its SDL representation.
*/
integer parse_integer(const std::string& sdlIntegerStr);

}// end namespace detail

//*****************************************************************************
// In-header Implementations:

inline real load_real(const std::string& sdlRealStr)
{
	return detail::parse_real(sdlRealStr);
}

inline integer load_integer(const std::string& sdlIntegerStr)
{
	return detail::parse_integer(sdlIntegerStr);
}

namespace detail
{

inline real parse_real(const std::string& sdlRealStr)
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

inline integer parse_integer(const std::string& sdlIntegerStr)
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

}// end namespace detail

}// end namespace ph::sdl
