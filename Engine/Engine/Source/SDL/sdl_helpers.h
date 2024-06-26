#pragma once

/*! @file

@brief Low-level helpers for SDL.
Helpers are in an additional `sdl` namespace.
*/

#include "SDL/sdl_fwd.h"
#include "Math/math_fwd.h"
#include "Utility/traits.h"
#include "Utility/TSpan.h"
#include "SDL/ESdlTypeCategory.h"
#include "SDL/ESdlDataType.h"

#include <Common/primitive_type.h>

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <concepts>

namespace ph::sdl
{

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

template<typename NumberType, std::size_t EXTENT = std::dynamic_extent>
void load_numbers(std::string_view sdlNumbersStr, TSpan<NumberType, EXTENT> out_numbers);

template<typename Element>
math::TVector2<Element> load_vector2(std::string_view sdlVec2Str);

template<typename Element>
math::TVector3<Element> load_vector3(std::string_view sdlVec3Str);

template<typename Element>
math::TVector4<Element> load_vector4(std::string_view sdlVec4Str);

template<typename Element>
math::TQuaternion<Element> load_quaternion(std::string_view sdlQuatStr);

template<typename NumberType>
std::vector<NumberType> load_number_array(std::string_view sdlNumberArrayStr);

template<typename Element>
std::vector<math::TVector3<Element>> load_vector3_array(std::string_view sdlVec3ArrayStr);

/*!
@param value The value to save.
@param out_str The string to append the saved data to.
*/
void save_real(real value, std::string& out_str);

/*!
@param value The value to save.
@param out_str The string to append the saved data to.
*/
void save_integer(integer value, std::string& out_str);

/*! @brief Converts a floating-point number to its SDL representation.
Supports `ph::real`, `float`, `double`, and `long double`.
@param value The value to save.
@param out_str The string to append the saved data to.
*/
template<typename FloatType>
void save_float(FloatType value, std::string& out_str);

/*! @brief Converts a integer number to its SDL representation.
Supports `ph::real`, `float`, `double`, and `long double`.
@param value The value to save.
@param out_str The string to append the saved data to.
*/
template<typename IntType>
void save_int(IntType value, std::string& out_str);

/*! @brief Converts a number to its SDL representation.
Accepts all types supported by save_float() and save_int().
@param value The value to save.
@param out_str The string to append the saved data to.
*/
template<typename NumberType>
void save_number(NumberType value, std::string& out_str);

/*!
@param numbers The values to save.
@param out_str The string to append the saved data to.
*/
template<typename NumberType, std::size_t EXTENT = std::dynamic_extent>
void save_numbers(TSpanView<NumberType, EXTENT> numbers, std::string& out_str);

/*!
@param value The value to save.
@param out_str The string to append the saved data to.
*/
template<typename Element>
void save_vector2(const math::TVector2<Element>& value, std::string& out_str);

/*!
@param value The value to save.
@param out_str The string to append the saved data to.
*/
template<typename Element>
void save_vector3(const math::TVector3<Element>& value, std::string& out_str);

/*!
@param value The value to save.
@param out_str The string to append the saved data to.
*/
template<typename Element>
void save_vector4(const math::TVector4<Element>& value, std::string& out_str);

/*!
@param value The value to save.
@param out_str The string to append the saved data to.
*/
template<typename Element>
void save_quaternion(const math::TQuaternion<Element>& value, std::string& out_str);

/*!
@param values The values to save.
@param out_str The string to append the saved data to.
*/
template<typename NumberType>
void save_number_array(TSpanView<NumberType> values, std::string& out_str);

/*!
@param values The values to save.
@param out_str The string to append the saved data to.
*/
template<typename Element>
void save_vector3_array(TSpanView<math::TVector3<Element>> values, std::string& out_str);

/*! @brief Save the identity of the field to output clause.
*/
void save_field_id(const SdlField* sdlField, SdlOutputClause& clause);

/*! @brief Generate a human-readable name for the SDL types.
These helpers allow input types to be null.
*/
///@{
std::string gen_pretty_name(const SdlClass* clazz);
std::string gen_pretty_name(const SdlStruct* ztruct);
std::string gen_pretty_name(const SdlField* field);
std::string gen_pretty_name(const SdlFunction* func);
std::string gen_pretty_name(const SdlClass* clazz, const SdlField* field);
///@}

/*! @brief Get a list of callable functions from the class.
@return A list of pairs containing {callable function, original parent class}.
*/
auto get_all_callable_functions(const SdlClass* callableParentClass)
-> std::vector<std::pair<const SdlFunction*, const SdlClass*>>;

/*! @brief Statically gets the SDL category of @p T.
@tparam T Type that category information is going to be extracted from.
@return Category of @p T. The result is ESdlTypeCategory::Unspecified if category
information does not exist, or @p T is not an @p ISdlResource.
*/
template<typename T>
constexpr ESdlTypeCategory category_of();

template<std::integral IntType>
constexpr ESdlDataType int_type_of();

template<std::floating_point FloatType>
constexpr ESdlDataType float_type_of();

template<CNumber NumberType>
constexpr ESdlDataType number_type_of();

template<typename T>
constexpr ESdlDataType resource_type_of();

/*! @brief Cast between SDL resource types.
Cast the input SDL resource instance of @p SrcType to an instance of @p DstType. Each of the input types
can possibly be `const` qualified. This is a stricter cast than standard `dynamic_cast` as both input
and output pointer must not be null, and it will throw if the cast cannot be done.
@param srcResource The resource to be casted.
@return The casted resource. Never null.
@exception SdlException If @p srcResource cannot be casted to @p DstType.
*/
template<typename DstType, typename SrcType>
DstType* cast_to(SrcType* srcResource);

/*!
SDL names are commonly being lower-case and separated by dashes (also known as
Kebab Case). These helpers transform SDL names into various forms.
*/
///@{

/*! @brief Make a standard SDL name to canonical capitalized form.
Make the SDL name capitalized and separated by spaces.
Example: "some-sdl-name" will be "Some Sdl Name".
*/
std::string name_to_title_case(std::string_view sdlName);

/*! @brief Make a standard SDL name to camel case.
Example: "some-sdl-name" will be "someSdlName" (if @p capitalizedFront is false).
*/
std::string name_to_camel_case(std::string_view sdlName, bool capitalizedFront = false);

/*! @brief Make a standard SDL name to snake case.
Example: "some-sdl-name" will be "some_sdl_name".
*/
std::string name_to_snake_case(std::string_view sdlName);

///@}

}// end namespace ph::sdl

#include "SDL/sdl_helpers.ipp"
