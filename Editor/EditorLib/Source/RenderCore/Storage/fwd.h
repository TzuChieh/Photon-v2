#pragma once

#include <type_traits>
#include <concepts>

namespace ph::editor
{

template<typename T>
concept CHandleDispatcher = 
	std::is_default_constructible_v<T> &&
	std::is_nothrow_move_constructible_v<T> &&
	std::is_nothrow_move_assignable_v<T> &&
	requires (T t)
	{
		typename T::HandleType;
		{ t.dispatchOne() } -> std::same_as<typename T::HandleType>;
		{ t.returnOne(typename T::HandleType{}) } -> std::same_as<void>;
	};

}// end namespace ph::editor
