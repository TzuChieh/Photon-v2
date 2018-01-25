#pragma once

#include <type_traits>

namespace ph
{

/*
	Checks whether the involved types has a multiply operator capable of 
	producing A * B = C with corresponding instances.
*/

template<typename A, typename B, typename C, typename Dummy = void>
struct has_multiply_operator : 
	std::false_type {};

template<typename A, typename B, typename C>
struct has_multiply_operator<A, B, C, 
	typename std::enable_if<std::is_same<decltype(A{} * B{}), C>::value>::type> : 
	std::true_type {};

}// end namespace ph