#pragma once

#include "Common/config.h"

#include <type_traits>
#include <cstddef>

namespace ph
{

namespace detail
{

template<typename T, std::size_t /* unused */>
class TFunctor final
{
	// Correct function signature will instantiate the specialized type. If this type is selected
	// instead, notify the user about the ill-formed function signature
	static_assert(std::is_function_v<T>,
		"Invalid function signature.");
};

template<typename R, typename... Args, std::size_t NUM_BYTES>
class TFunctor<R(Args...), NUM_BYTES> final
{
public:

private:
	std::size_t a = NUM_BYTES;
};

}// end namespace detail

template<typename FunctionSignature>
using TFunctor = detail::TFunctor<FunctionSignature, PH_FUNCTOR_STORAGE_SIZE_IN_BYTES>;

}// end namespace ph
