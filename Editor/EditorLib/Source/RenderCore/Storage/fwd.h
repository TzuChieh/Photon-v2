#pragma once

#include <concepts>

namespace ph::editor
{

template<typename T>
concept CHandleDispatcher = requires (T t)
{
	typename T::HandleType;
	{ t.dispatchOne() } -> std::same_as<typename T::HandleType>;
	{ t.returnOne(typename T::HandleType{}) } -> std::same_as<void>;
};

}// end namespace ph::editor
