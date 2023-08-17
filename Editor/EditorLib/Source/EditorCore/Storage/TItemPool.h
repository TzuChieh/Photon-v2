#pragma once

#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/assertion.h>
#include <Utility/exception.h>
#include <Utility/utility.h>

#include <cstddef>
#include <vector>
#include <utility>
#include <memory>
#include <iterator>
#include <type_traits>

namespace ph::editor
{

template<typename Item, typename Index = std::size_t, typename Generation = Index>
class TItemPool final
{
public:
	/*! If `Item` is a polymorphic type, then `Item` itself along with all its bases can form a
	valid handle type. These compatible handles can be used for accessing items stored in the pool
	(items obtained have the same item type indicated by the handle, for type safety).
	*/
	template<typename ItemType> requires std::is_scalar_v<Item> || CBase<ItemType, Item>
	using TCompatibleHandleType = TWeakHandle<ItemType, Index, Generation>;

	using HandleType = TCompatibleHandleType<Item>;

public:
	template<bool IS_CONST>
	class TIterator
	{
	public:
		using ItemType = std::conditional_t<IS_CONST, const Item, Item>;
		using PoolType = std::conditional_t<IS_CONST, const TItemPool, TItemPool>;

		// Standard iterator traits
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = ItemType;
		using difference_type = std::ptrdiff_t;
		using pointer = ItemType*;
		using reference = ItemType&;

		// Default constructible
		inline TIterator() = default;

		inline TIterator(PoolType* const pool, Index currentIdx)
			: m_pool(pool)
			, m_currentIdx(currentIdx)
		{}

		// Dereferenceable
		inline reference operator * () const
		{
			PH_ASSERT(m_pool);
			PH_ASSERT_LT(m_currentIdx, m_pool->m_storage.size());
			PH_ASSERT(!m_pool->m_storageStates[m_currentIdx].isFreed);

			return m_pool->m_storage[m_currentIdx];
		}

		// Pre-incrementable
		inline TIterator& operator ++ ()
		{
			PH_ASSERT(m_pool);

			++m_currentIdx;
			m_currentIdx = m_pool->nextItemBeginIndex(m_currentIdx);
			return *this;
		}

		// Post-incrementable
		inline TIterator operator ++ (int)
		{
			TIterator current = *this;
			++(*this);
			return current;
		}

		// Pre-decrementable
		inline TIterator& operator -- ()
		{
			PH_ASSERT(m_pool);

			// Using current index as end index is equivalent to decrement by 1
			m_currentIdx = m_pool->previousItemEndIndex(m_currentIdx);
			PH_ASSERT_GT(m_currentIdx, 0);// If this method can be called, there must exist a previous
			--m_currentIdx;               // item and `m_currentIdx` is impossible to reach 0 since
			return *this;                 // it is used as an (exclusive) end index here.
		}

		// Post-decrementable
		inline TIterator operator -- (int)
		{
			TIterator current = *this;
			--(*this);
			return current;
		}

		// Equality
		inline bool operator == (const TIterator& rhs)
		{
			return m_currentIdx == rhs.m_currentIdx && m_pool == rhs.m_pool;
		}

		// Inequality
		inline bool operator != (const TIterator& rhs)
		{
			return !(*this == rhs);
		}

	private:
		PoolType* m_pool = nullptr;
		Index m_currentIdx = HandleType::INVALID_INDEX;
	};

public:
	using IteratorType = TIterator<false>;
	using ConstIteratorType = TIterator<false>;

	/*!
	Complexity: Amortized O(1). O(1) if `hasFreeSpace()` returns true.
	*/
	inline HandleType add(Item item)
	{
		PH_ASSERT_EQ(m_storage.size(), m_storageStates.size());

		Index freeIdx = HandleType::INVALID_INDEX;

		// Create new storage space
		if(m_freeIndices.empty())
		{
			m_storage.push_back(std::move(item));
			m_storageStates.push_back({.isFreed = false});
			freeIdx = m_storage.size() - 1;
		}
		// Existing space available, get it and update free list head
		else
		{
			freeIdx = m_freeIndices.back();
			m_freeIndices.pop_back();
			PH_ASSERT_LT(freeIdx, m_storage.size());

			// `Item` was manually destroyed. No need for storing the returned pointer nor using
			// `std::launder()` on each use (same object type with exactly the same storage location), 
			// see C++ standard [basic.life] section 8 (https://timsong-cpp.github.io/cppwp/n4659/basic.life#8).
			std::construct_at(&m_storage[freeIdx], std::move(item));

			PH_ASSERT(m_storageStates[freeIdx].isFreed);
			m_storageStates[freeIdx].isFreed = false;
		}

		return HandleType(freeIdx, m_storageStates[freeIdx].generation);
	}

	/*!
	Complexity: O(1).
	*/
	template<typename ItemType>
	inline void remove(const TCompatibleHandleType<ItemType>& handle)
	{
		if(!isFresh(handle))
		{
			throw_formatted<IllegalOperationException>(
				"removing item with stale handle ({})",
				handle.toString());
		}

		const Index freeIdx = handle.getIndex();
		PH_ASSERT_LT(freeIdx, m_storage.size());

		std::destroy_at(&m_storage[freeIdx]);

		// Generally should not happen: the generation counter increases on each item removal, using
		// the same handle to call this method again will result in `isFresh()` being false which
		// will throw. If this fails, could be generation collision or bad handles (from wrong pool).
		PH_ASSERT(!m_storageStates[freeIdx].isFreed);
		m_storageStates[freeIdx].isFreed = true;
		++m_storageStates[freeIdx].generation;
		m_freeIndices.push_back(freeIdx);
	}

	/*!
	Complexity: O(1).
	*/
	template<typename ItemType>
	inline ItemType* get(const TCompatibleHandleType<ItemType>& handle)
	{
		return isFresh(handle) ? &(m_storage[handle.getIndex()]) : nullptr;
	}

	/*!
	Complexity: O(1).
	*/
	template<typename ItemType>
	inline const ItemType* get(const TCompatibleHandleType<ItemType>& handle) const
	{
		return isFresh(handle) ? &(m_storage[handle.getIndex()]) : nullptr;
	}

	inline Index numItems() const
	{
		PH_ASSERT_LE(numFreeSpace(), m_storage.size());
		return m_storage.size() - numFreeSpace();
	}

	inline Index numFreeSpace() const
	{
		return m_freeIndices.size();
	}

	inline bool isEmpty() const
	{
		return numItems() == 0;
	}

	inline bool hasFreeSpace() const
	{
		return numFreeSpace() > 0;
	}

	template<typename ItemType>
	inline bool isFresh(const TCompatibleHandleType<ItemType>& handle) const
	{
		return handle.getIndex() < m_storageStates.size() &&
		       handle.getGeneration() == m_storageStates[handle.getIndex()].generation;
	}

	inline IteratorType begin()
	{
		return IteratorType(this, nextItemBeginIndex(0));
	}

	inline IteratorType end()
	{
		return IteratorType(this, m_storage.size());
	}

	inline ConstIteratorType cbegin()
	{
		return IteratorType(this, nextItemBeginIndex(0));
	}

	inline ConstIteratorType cend()
	{
		return IteratorType(this, m_storage.size());
	}

private:
	inline Index nextItemBeginIndex(const Index beginIdx) const
	{
		// If failed, most likely be using an out-of-range iterator
		PH_ASSERT_IN_RANGE_INCLUSIVE(beginIdx, 0, m_storage.size());

		Index itemBeginIdx = beginIdx;
		while(itemBeginIdx < m_storage.size())
		{
			if(m_storageStates[itemBeginIdx].isFreed)
			{
				++itemBeginIdx;
			}
			else
			{
				break;
			}
		}
		return itemBeginIdx;
	}

	inline Index previousItemEndIndex(const Index endIdx) const
	{
		// If failed, most likely be using an out-of-range iterator
		PH_ASSERT_IN_RANGE_INCLUSIVE(endIdx, 0, m_storage.size());

		Index itemEndIdx = endIdx;
		while(itemEndIdx > 0)
		{
			if(m_storageStates[itemEndIdx - 1].isFreed)
			{
				--itemEndIdx;
			}
			else
			{
				break;
			}
		}
		return itemEndIdx;
	}

private:
	struct StorageState
	{
		Index generation = 0;
		bool isFreed = true;
	};

	std::vector<Item> m_storage;
	std::vector<StorageState> m_storageStates;
	std::vector<Index> m_freeIndices;
};

}// end namespace ph::editor
