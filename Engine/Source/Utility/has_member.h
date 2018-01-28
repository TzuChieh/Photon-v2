#pragma once

#include <type_traits>
#include <utility>

namespace ph
{

/******************************************************************************
	Checks whether the involved types has a multiply operator and is capable
	of assigning the result into the third type, 
	i.e., able to do C = A * B with corresponding instances.
*/

template<typename A, typename B, typename C, typename = void>
struct has_multiply_operator : std::false_type {};

template<typename A, typename B, typename C>
struct has_multiply_operator
<
	A, B, C, 
	std::enable_if_t
	<
		std::is_convertible_v<decltype(std::declval<A>() * std::declval<B>()), C>
	>
> : std::true_type {};

/******************************************************************************
	Checks whether the involved types has a add operator and is capable
	of assigning the result into the third type,
	i.e., able to do C = A + B with corresponding instances.
*/

template<typename A, typename B, typename C, typename = void>
struct has_add_operator : std::false_type {};

template<typename A, typename B, typename C>
struct has_add_operator
<
	A, B, C, 
	std::enable_if_t
	<
		std::is_convertible_v<decltype(std::declval<A>() + std::declval<B>()), C>
	>
> : std::true_type {};

}// end namespace ph