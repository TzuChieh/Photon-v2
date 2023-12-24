#pragma once

#include <type_traits>
#include <concepts>
#include <utility>

namespace ph::editor
{

template<typename T>
concept CWeakHandle = requires
{
	typename T::WeakHandleTag;
};

template<typename T>
concept CHandleDispatcher = 
	std::is_default_constructible_v<T> &&
	std::is_nothrow_move_constructible_v<T> &&
	requires (T t)
	{
		typename T::HandleType;
		{ t.dispatchOne() } -> std::same_as<typename T::HandleType>;
		{ t.returnOne(std::declval<typename T::HandleType>()) } -> std::same_as<void>;
	};

template<typename ItemInterface, CWeakHandle Handle>
class TItemPoolInterface;

template<typename Item, typename Index, typename Generation>
class TWeakHandle;

template<typename ItemInterface, typename Index, typename Generation>
class TStrongHandle;

}// end namespace ph::editor
