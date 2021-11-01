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
class SdlOutputPayload;

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

template<typename NumberType>
std::vector<NumberType> load_number_array(const std::string& sdlNumberArrayStr);

std::vector<math::Vector3R> load_vector3_array(const std::string& sdlVector3ArrayStr);

void save_real(real value, std::string* out_str);
void save_integer(integer value, std::string* out_str);

/*! @brief Converts a floating-point number to its SDL representation.

Supports `ph::real`, `float`, `double`, and `long double`.
*/
template<typename FloatType>
void save_float(FloatType value, std::string* out_str);

/*! @brief Converts a integer number to its SDL representation.

Supports `ph::real`, `float`, `double`, and `long double`.
*/
template<typename IntType>
void save_int(IntType value, std::string* out_str);

/*! @brief Converts a number to its SDL representation.

Accepts all types supported by save_float() and save_int().
*/
template<typename NumberType>
void save_number(NumberType value, std::string* out_str);

template<typename Element>
void save_vector2(const math::TVector2<Element>& value, std::string* out_str);

void save_vector3(const math::Vector3R& value, std::string* out_str);
void save_quaternion(const math::QuaternionR& value, std::string* out_str);
void save_real_array(const std::vector<real>& values, std::string* out_str);
void save_vector3_array(const std::vector<math::Vector3R>& values, std::string* out_str);

void save_field_id(const SdlField* sdlField, SdlOutputPayload& payload);

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
		return string_utils::parse_int<IntType>(sdlIntStr);
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

inline void save_real(const real value, std::string* const out_str)
{
	save_float<real>(value, out_str);
}

inline void save_integer(const integer value, std::string* const out_str)
{
	save_int<integer>(value, out_str);
}

template<typename FloatType>
inline void save_float(const FloatType value, std::string* const out_str)
{
	constexpr std::size_t BUFFER_SIZE = 32;

	PH_ASSERT(out_str);

	try
	{
		out_str->resize(BUFFER_SIZE);

		const std::size_t actualStrSize = string_utils::stringify_float<FloatType>(
			value, out_str->data(), BUFFER_SIZE);

		out_str->resize(actualStrSize);
	}
	catch(const std::exception& e)
	{
		throw SdlSaveError("on saving floating-point value -> " + std::string(e.what()));
	}
}

template<typename IntType>
inline void save_int(const IntType value, std::string* const out_str)
{
	constexpr std::size_t BUFFER_SIZE = 32;

	PH_ASSERT(out_str);

	try
	{
		out_str->resize(BUFFER_SIZE);

		const std::size_t actualStrSize = string_utils::stringify_int<IntType>(
			value, out_str->data(), BUFFER_SIZE);

		out_str->resize(actualStrSize);
	}
	catch(const std::exception& e)
	{
		throw SdlSaveError("on saving integer value -> " + std::string(e.what()));
	}
}

template<typename NumberType>
inline void save_number(const NumberType value, std::string* const out_str)
{
	if constexpr(std::is_floating_point_v<NumberType>)
	{
		save_float<NumberType>(value, out_str);
	}
	else
	{
		static_assert(std::is_integral_v<NumberType>);

		save_int<NumberType>(value, out_str);
	}
}

template<typename T>
inline constexpr ETypeCategory category_of()
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
