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

/*! @brief Check if instances of types can be subtracted.

Checks whether the instances of the involved types can be subtracted
and is capable of assigning the result into an instance of the third type,
i.e., able to do C = A - B with corresponding instances.
*/
template<typename A, typename B, typename C>
concept CCanSubtract = requires (A a, B b, C c)
{
	c = a - b;
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

/*! @brief Check if instances of types can be divided.

Checks whether the instances of the involved types can be divided
and is capable of assigning the result into an instance of the third type,
i.e., able to do C = A / B with corresponding instances.
*/
template<typename A, typename B, typename C>
concept CCanDivide = requires (A a, B b, C c)
{
	c = a / b;
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

/*! @brief Whether the type is a scoped/unscoped enum.
*/
template<typename EnumType>
concept CEnum = std::is_enum_v<EnumType>;

template<typename EnumType>
concept CEnumWithSizeInfo = CEnum<EnumType> && requires
{
	{ EnumType::SIZE } -> std::convertible_to<EnumType>;
};

template<typename NumberType>
concept CNumber = std::is_arithmetic_v<NumberType>;

template<typename TypeA, typename TypeB>
concept CSame = std::same_as<TypeA, TypeB>;

template<typename TypeA, typename TypeB>
concept CNotSame = !CSame<TypeA, TypeB>;

/*! @brief Checks whether @p DerivedType is derived from @p BaseType.
The result is also `true` if both types are the same; `false` if one of them is a primitive type.
*/
template<typename DerivedType, typename BaseType>
concept CDerived = std::derived_from<DerivedType, BaseType>;

/*! @brief Checks whether @p BaseType is a base of @p DerivedType.
The result is also `true` if both types are the same; `false` if one of them is a primitive type.
*/
template<typename BaseType, typename DerivedType>
concept CBase = std::derived_from<DerivedType, BaseType>;

}// end namespace ph
