#pragma once

#include "EditorCore/Storage/fwd.h"

#include <Utility/traits.h>
#include <Utility/utility.h>
#include <Math/hash.h>

#include <cstddef>
#include <limits>
#include <string>
#include <functional>

namespace ph::editor
{

template<typename Item, typename Index = std::size_t, typename Generation = Index>
class TWeakHandle final
{
public:
	using WeakHandleTag = void;
	using ItemType = Item;
	using IndexType = Index;
	using GenerationType = Generation;

	inline static constexpr auto INVALID_INDEX = std::numeric_limits<Index>::max();
	inline static constexpr auto INVALID_GENERATION = 0;

	PH_DEFINE_INLINE_RULE_OF_5_MEMBERS(TWeakHandle);

	inline TWeakHandle(Index itemIdx, Generation itemGeneration)
		: m_itemIdx(itemIdx)
		, m_itemGeneration(itemGeneration)
	{}

	/*!
	Implicit conversion from a derived item type to base item type is allowed. Modeling the same
	behavior as pointers.
	*/
	template<CDerived<Item> DerivedItem>
	inline TWeakHandle(const TWeakHandle<DerivedItem, Index, Generation>& otherHandle)
		: TWeakHandle(otherHandle.getIndex(), otherHandle.getGeneration())
	{}

	inline Index getIndex() const
	{
		return m_itemIdx;
	}

	inline Generation getGeneration() const
	{
		return m_itemGeneration;
	}

	/*!
	@return Whether the handle points to nothing.
	*/
	inline bool isEmpty() const
	{
		return m_itemIdx == INVALID_INDEX || m_itemGeneration == INVALID_GENERATION;
	}

	inline std::string toString() const
	{
		return "index=" + std::to_string(m_itemIdx) + ", generation=" + std::to_string(m_itemGeneration);
	}

	/*!
	@return Whether the handle points to something. Equivalent to checking if the handle is not empty.
	*/
	inline operator bool () const
	{
		return !isEmpty();
	}

	inline bool operator == (const TWeakHandle& rhs) const = default;

	/*! @brief Get the next generation given a current generation.
	@note We do not provide a method to directly "tick" a handle's generation since that would
	partially defeat the purpose of using a generation counter--that may encourage the user to
	tick a handle's generation until it is valid. Getting the next generation, however, is useful
	for many library routines so we provide a static method here.
	*/
	inline static constexpr Generation nextGeneration(const Generation currentGeneration)
	{
		Index nextGen = currentGeneration + 1;
		if(nextGen == INVALID_GENERATION)
		{
			++nextGen;
		}
		return nextGen;
	}

private:
	Index m_itemIdx = INVALID_INDEX;
	Generation m_itemGeneration = INVALID_GENERATION;
};

}// end namespace ph::editor

namespace std
{

template<ph::editor::CWeakHandle Handle>
struct hash<Handle>
{
	inline std::size_t operator () (const Handle& handle) const
	{
		return ph::math::murmur3_32(handle, 0);
	}
};

}// end namespace std
