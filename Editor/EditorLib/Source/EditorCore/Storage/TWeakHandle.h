#pragma once

#include <Utility/traits.h>
#include <Utility/utility.h>

#include <cstddef>
#include <limits>
#include <string>

namespace ph::editor
{

template<typename Item, typename Index = std::size_t, typename Generation = Index>
class TWeakHandle final
{
public:
	using IsWeakHandle = void;
	using ItemType = Item;
	using IndexType = Index;
	using GenerationType = Generation;

	inline static constexpr auto INVALID_INDEX = std::numeric_limits<Index>::max();

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
		return m_itemIdx == INVALID_INDEX;
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

private:
	Index m_itemIdx = INVALID_INDEX;
	Generation m_itemGeneration = 0;
};

}// end namespace ph::editor
