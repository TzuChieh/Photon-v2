#pragma once

/*! @file

@brief Low-level helpers for SDL.
Helpers are in an additional `sdl` namespace.
*/

#include "Common/primitive_type.h"
#include "Common/assertion.h"
#include "Math/TVector3.h"
#include "Math/TVector2.h"
#include "Math/TQuaternion.h"
#include "Utility/string_utils.h"
#include "DataIO/SDL/ESdlTypeCategory.h"
#include "DataIO/SDL/ESdlDataType.h"

#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <concepts>

namespace ph
{

class SdlClass;
class SdlField;
class SdlFunction;
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
void save_float(FloatType value, std::string* out_str, std::size_t maxChars = 32);

/*! @brief Converts a integer number to its SDL representation.

Supports `ph::real`, `float`, `double`, and `long double`.
*/
template<typename IntType>
void save_int(IntType value, std::string* out_str, std::size_t maxChars = 32);

/*! @brief Converts a number to its SDL representation.

Accepts all types supported by save_float() and save_int().
*/
template<typename NumberType>
void save_number(NumberType value, std::string* out_str, std::size_t maxChars = 32);

template<typename Element>
void save_vector2(const math::TVector2<Element>& value, std::string* out_str);

void save_vector3(const math::Vector3R& value, std::string* out_str);
void save_quaternion(const math::QuaternionR& value, std::string* out_str);

template<typename NumberType>
void save_number_array(const std::vector<NumberType>& values, std::string* out_str);

void save_vector3_array(const std::vector<math::Vector3R>& values, std::string* out_str);

/*! @brief Save the identity of the field to output payload.
*/
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

template<typename T>
constexpr ESdlDataType resource_type_of();

/*! @brief Cast between SDL resource types.
Cast the input SDL resource instance of `SrcType` to an instance of `DstType`. Each of the input types
can possibly be `const` qualified.
@param srcResource The resource to be casted.
@return The casted resource. Never null.
*/
template<typename DstType, typename SrcType>
DstType* cast_to(SrcType* srcResource);

}// end namespace ph::sdl

#include "DataIO/SDL/sdl_helpers.ipp"
