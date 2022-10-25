#pragma once

#include <type_traits>
#include <utility>
#include <cstddef>

namespace ph
{

// TODO: variants that ignores return type

/*! @brief Check if instances of types can be added together.

Checks whether the instances of the involved types can be added together
and is capable of assigning the result into an instance of the third type,
i.e., able to do C = A + B with corresponding instances.
*/
template<typename A, typename B, typename C>
concept CCanAdd = requires (A a, B b, C c)
{
	c = a + b;
};

/*! @brief Check if instances of types can be multiplied together.

Checks whether the instances of the involved types can be multiplied together
and is capable of assigning the result into an instance of the third type,
i.e., able to do C = A * B with corresponding instances.
*/
template<typename A, typename B, typename C>
concept CCanMultiply = requires (A a, B b, C c)
{
	c = a * b;
};

/*! @brief Check if object conversion can be made.

Checks whether an object of type Built can be made from an object of
type From. Both implicit and explicit conversions are considered.
*/
///@{

/*! @brief Return type if the result is false.
*/
template<typename Built, typename From, typename = void>
struct IsBuildable : std::false_type {};

/*! @brief Return type if the result is true.
*/
template<typename Built, typename From>
struct IsBuildable
<
	Built, From,
	std::enable_if_t
	<
		std::is_convertible_v<From, Built> || 
		std::is_constructible_v<Built, From>
	>
> : std::true_type {};
///@}

template<typename ObjType>
concept CSubscriptable = requires (ObjType obj, std::size_t index)
{
	obj[index];
};

template<typename EnumType>
concept CIsEnum = std::is_enum_v<EnumType>;

template<typename NumberType>
concept CIsNumber = std::is_arithmetic_v<NumberType>;

}// end namespace ph
