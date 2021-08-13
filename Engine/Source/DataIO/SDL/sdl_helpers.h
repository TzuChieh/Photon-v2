#pragma once

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Math/TQuaternion.h"
#include "DataIO/SDL/sdl_exceptions.h"
#include "DataIO/SDL/ETypeCategory.h"
#include "Utility/string_utils.h"

#include <string>
#include <string_view>
#include <array>
#include <exception>

namespace ph
{

class SdlClass;
class SdlField;
class SdlFunction;
class ISdlResource;

}// end namespace ph

namespace ph::sdl
{

// TODO: templatize vec3, quat related funcs

real load_real(std::string_view sdlRealStr);
integer load_integer(std::string_view sdlIntegerStr);

/*! @brief Returns a floating-point number by processing its SDL representation.

Supports `ph::real`, `float`, `double`, and `long double`.
*/
template<typename FloatType>
FloatType load_float(std::string_view sdlFloatStr);

/*! @brief Returns a integer number by processing its SDL representation.

Supports `ph::integer` and all signed and unsigned standard integer types.
*/
template<typename IntType>
IntType load_int(std::string_view sdlIntStr);

/*! @brief Returns a number by processing its SDL representation.

Accepts all types supported by load_float() and load_int().
*/
template<typename NumberType>
NumberType load_number(std::string_view sdlNumberStr);

template<typename Element>
math::TVector2<Element> load_vector2(const std::string& sdlVector2Str);

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

/*! @brief Statically gets the category of @p T.

@tparam T Type that category information is going to be extracted from.
@return Category of @p T. The result is ETypeCategory::UNSPECIFIED if category
information does not exist, or @p T is not an @p ISdlResource.
*/
template<typename T>
constexpr ETypeCategory category_of();

//template<typename T>
//void init_to_default(T& resource);

namespace detail
{

/*! @brief Check if category information can be obtained statically.

The result is true if the static member variable T::CATEGORY exists,
otherwise the result is false.
*/
///@{

/*! @brief Return type if the result is false.
*/
template<typename T, typename = void>
struct HasStaticCategoryInfo : std::false_type {};

/*! @brief Return type if the result is true.
*/
template<typename T>
struct HasStaticCategoryInfo
<
	T,
	std::enable_if_t
	<
		// Check equality of types with cv and ref removed just to be robust.
		// (TODO: use std::remove_cvref to simplify)
		std::is_same_v
		<
			std::remove_cv_t<std::remove_reference_t<decltype(T::CATEGORY)>>, 
			ETypeCategory
		>
	>
> : std::true_type {};
///@}

}// end namespace detail

//*****************************************************************************
// In-header Implementations:

template<typename FloatType>
inline FloatType load_float(const std::string_view sdlFloatStr)
{
	try
	{
		return string_utils::parse_float<FloatType>(sdlFloatStr);
	}
	catch(const std::exception& e)
	{
		throw SdlLoadError("on loading floating-point value -> " + std::string(e.what()));
	}
}

template<typename IntType>
inline IntType load_int(const std::string_view sdlIntStr)
{
	try
	{
		return detail::parse_int<IntType>(sdlIntStr);
	}
	catch(const std::exception& e)
	{
		throw SdlLoadError("on loading integer value -> " + std::string(e.what()));
	}
}

template<typename NumberType>
inline NumberType load_number(const std::string_view sdlNumberStr)
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		return load_float<NumberType>(sdlNumberStr);
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);

		return load_int<NumberType>(sdlNumberStr);
	}
}

inline real load_real(const std::string_view sdlRealStr)
{
	return load_float<real>(sdlRealStr);
}

inline integer load_integer(const std::string_view sdlIntegerStr)
{
	return load_int<integer>(sdlIntegerStr);
}

template<typename T>
constexpr ETypeCategory category_of()
{
	if constexpr(std::is_base_of_v<ISdlResource, T> && detail::HasStaticCategoryInfo<T>::value)
	{
		return T::CATEGORY;
	}
	else
	{
		return ETypeCategory::UNSPECIFIED;
	}
}

}// end namespace ph::sdl

#include "DataIO/SDL/sdl_helpers.ipp"
