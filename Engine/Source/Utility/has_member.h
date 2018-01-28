#pragma once

#include <type_traits>
#include <utility>

namespace ph
{

/*
	Checks whether the involved types has a multiply operator capable of 
	producing A * B = C with corresponding instances.
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

}// end namespace ph