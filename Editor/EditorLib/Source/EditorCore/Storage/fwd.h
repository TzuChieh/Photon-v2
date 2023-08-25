#pragma once

#include <concepts>

namespace ph::editor
{

template<typename T>
concept CWeakHandle = requires
{
	typename T::WeakHandleTag;
	typename T::ItemType;
	typename T::IndexType;
	typename T::GenerationType;
	T::INVALID_INDEX;
	T::INVALID_GENERATION;
	{ T::nextGeneration(typename T::GenerationType{}) } -> std::same_as<typename T::GenerationType>;
};

template<typename ItemInterface, CWeakHandle Handle>
class TItemPoolInterface;

template<typename Item, typename Index, typename Generation>
class TWeakHandle;

template<typename ItemInterface, typename Index, typename Generation>
class TStrongHandle;

}// end namespace ph::editor
