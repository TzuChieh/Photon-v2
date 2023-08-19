#pragma once

#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/assertion.h>
#include <Common/memory.h>
#include <Common/os.h>
#include <Common/math_basics.h>
#include <Utility/exception.h>
#include <Utility/utility.h>

#include <cstddef>
#include <vector>
#include <utility>
#include <memory>
#include <iterator>
#include <type_traits>
#include <concepts>
#include <new>
#include <limits>
#include <numeric>

namespace ph::editor
{

template<typename Item, typename Index = std::size_t, typename Generation = Index>
class TItemPool final
{
	static_assert(std::move_constructible<Item>);

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
			PH_ASSERT_LT(m_currentIdx, m_pool->m_storageStates.size());
			PH_ASSERT(!m_pool->m_storageStates[m_currentIdx].isFreed);

			return m_pool->m_storageMemory.get()[m_currentIdx];
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

	inline TItemPool() = default;

	/*! @brief Copy items stored in `other` into this pool.
	Handles that were originally valid for `other` will no longer valid for this pool.
	*/
	inline TItemPool(const TItemPool& other) requires std::copy_constructible<Item>
		: TItemPool()
	{
		grow(other.capacity());

		other.forEachItem(
			[this](const Item* otherItem, Index /* idx */)
			{
				add(*otherItem);
			});
	}

	inline TItemPool(TItemPool&& other) noexcept = default;

	inline TItemPool& operator = (TItemPool rhs)
	{
		using std::swap;

		swap(*this, rhs);

		return *this;
	}

	inline ~TItemPool()
	{
		clear();

		// All storage spaces should be freed at the end
		PH_ASSERT_EQ(m_freeIndices.size(), m_storageStates.size());
	}

	/*!
	Complexity: Amortized O(1). O(1) if `hasFreeSpace()` returns true.
	*/
	inline HandleType add(Item item)
	{
		// Create new storage space
		if(m_freeIndices.empty())
		{
			if(capacity() == maxCapacity())
			{
				throw_formatted<OverflowException>(
					"Storage size will exceed the maximum amount Index type can hold (max={})",
					std::numeric_limits<Index>::max());
			}

			grow(nextCapacity(capacity()));
		}

		PH_ASSERT_GT(m_freeIndices.size(), 0);

		const Index freeIdx = m_freeIndices.back();
		PH_ASSERT_LT(freeIdx, m_storageStates.size());
		PH_ASSERT(m_storageStates[freeIdx].isFreed);

		// `Item` was manually destroyed. No need for storing the returned pointer nor using
		// `std::launder()` on each use (same object type with exactly the same storage location), 
		// see C++ standard [basic.life] section 8 (https://timsong-cpp.github.io/cppwp/n4659/basic.life#8).
		std::construct_at(m_storageMemory.get() + freeIdx, std::move(item));

		m_freeIndices.pop_back();
		m_storageStates[freeIdx].isFreed = false;

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

		const Index itemIdx = handle.getIndex();

		// Generally should not happen: the generation counter increases on each item removal, using
		// the same handle to call this method again will result in `isFresh()` being false which
		// will throw. If this fails, could be generation collision or bad handles (from wrong pool).
		PH_ASSERT(!m_storageStates[itemIdx].isFreed);

		removeItemAt(itemIdx);
	}

	inline void clear()
	{
		if(isEmpty())
		{
			return;
		}

		forEachItem(
			[this](Item* /* item */, Index idx)
			{
				removeItemAt(idx);
			});
	}

	/*!
	Complexity: O(1).
	*/
	template<typename ItemType>
	inline ItemType* get(const TCompatibleHandleType<ItemType>& handle)
	{
		return isFresh(handle) ? (m_storageMemory.get() + handle.getIndex()) : nullptr;
	}

	/*!
	Complexity: O(1).
	*/
	template<typename ItemType>
	inline const ItemType* get(const TCompatibleHandleType<ItemType>& handle) const
	{
		return isFresh(handle) ? (m_storageMemory.get() + handle.getIndex()) : nullptr;
	}

	inline Index numItems() const
	{
		PH_ASSERT_LE(numFreeSpace(), capacity());
		return capacity() - numFreeSpace();
	}

	inline Index numFreeSpace() const
	{
		return m_freeIndices.size();
	}

	inline Index capacity() const
	{
		PH_ASSERT_LE(m_storageStates.size(), maxCapacity());
		return static_cast<Index>(m_storageStates.size());
	}

	/*!
	@return Wether this pool contains any item.
	*/
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
		return IteratorType(this, capacity());
	}

	inline ConstIteratorType cbegin()
	{
		return IteratorType(this, nextItemBeginIndex(0));
	}

	inline ConstIteratorType cend()
	{
		return IteratorType(this, capacity());
	}

	inline static constexpr Index maxCapacity()
	{
		return std::numeric_limits<Index>::max();
	}

	inline friend void swap(TItemPool& first, TItemPool& second)
	{
		// Enable ADL
		using std::swap;

		swap(first.m_storageMemory, second.m_storageMemory);
		swap(first.m_storageStates, second.m_storageStates);
		swap(first.m_freeIndices, second.m_freeIndices);
	}

private:
	inline void removeItemAt(const Index itemIdx)
	{
		PH_ASSERT_LT(itemIdx, capacity());
		PH_ASSERT(!m_storageStates[itemIdx].isFreed);

		std::destroy_at(m_storageMemory.get() + itemIdx);
		
		m_storageStates[itemIdx].isFreed = true;
		m_storageStates[itemIdx].generation = nextGeneration(m_storageStates[itemIdx].generation);
		m_freeIndices.push_back(itemIdx);
	}

	inline void grow(const Index newCapacity)
	{
		const Index oldCapacity = capacity();
		PH_ASSERT_GT(newCapacity, oldCapacity);

		const auto requiredMemorySize = newCapacity * sizeof(Item);
		const auto alignmentSize = std::lcm(alignof(Item), os::get_L1_cache_line_size_in_bytes());
		const auto totalMemorySize = math::next_multiple(requiredMemorySize, alignmentSize);

		// Create new item storage and move item over

		TAlignedMemoryUniquePtr<Item> newStorageMemory = 
			make_aligned_memory<Item>(totalMemorySize, alignmentSize);
		if(!newStorageMemory)
		{
			throw std::bad_alloc();
		}

		forEachItem(
			[newItems = newStorageMemory.get()](Item* oldItem, Index idx)
			{
				std::construct_at(newItems + idx, std::move(*oldItem));
				std::destroy_at(oldItem);
			});

		// Extend storage states to cover new storage spaces
		m_storageStates.resize(newCapacity, StorageState{});

		// All new storage spaces are free. Indices are added backwards as free spaces are reused in
		// LIFO order, we want to use spaces closer to other items first.
		std::vector<Index> newFreeIndices;
		newFreeIndices.reserve(newCapacity - oldCapacity);
		for(Index n = newCapacity; n > oldCapacity; --n)
		{
			const Index newFreeIdx = n - 1;
			newFreeIndices.push_back(newFreeIdx);
		}
		m_freeIndices.insert(m_freeIndices.begin(), newFreeIndices.begin(), newFreeIndices.end());

		// Finally, get rid of the old item storage
		m_storageMemory = std::move(newStorageMemory);
	}

	template<typename PerItemStorageOp>
	inline void forEachItemStorage(PerItemStorageOp op)
	{
		for(Index itemIdx = 0; itemIdx < capacity(); ++itemIdx)
		{
			Item* item = m_storageMemory.get() + itemIdx;
			op(item, itemIdx);
		}
	}

	template<typename PerItemStorageOp>
	inline void forEachItemStorage(PerItemStorageOp op) const
	{
		for(Index itemIdx = 0; itemIdx < capacity(); ++itemIdx)
		{
			const Item* item = m_storageMemory.get() + itemIdx;
			op(item, itemIdx);
		}
	}

	template<typename PerItemOp>
	inline void forEachItem(PerItemOp op)
	{
		forEachItemStorage(
			[op, this](Item* item, Index idx)
			{
				if(m_storageStates[idx].isFreed)
				{
					return;
				}

				op(item, idx);
			});
	}

	template<typename PerItemOp>
	inline void forEachItem(PerItemOp op) const
	{
		forEachItemStorage(
			[op, this](const Item* item, Index idx)
			{
				if(m_storageStates[idx].isFreed)
				{
					return;
				}

				op(item, idx);
			});
	}

	inline Index nextItemBeginIndex(const Index beginIdx) const
	{
		// If failed, most likely be using an out-of-range iterator
		PH_ASSERT_IN_RANGE_INCLUSIVE(beginIdx, 0, capacity());

		Index itemBeginIdx = beginIdx;
		while(itemBeginIdx < capacity())
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
		PH_ASSERT_IN_RANGE_INCLUSIVE(endIdx, 0, capacity());

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

	inline static constexpr Index nextGeneration(const Index currentGeneration)
	{
		Index nextGen = currentGeneration + 1;
		if(nextGen == HandleType::INVALID_GENERATION)
		{
			++nextGen;
		}
		return nextGen;
	}

	inline static constexpr Index nextCapacity(const Index currentCapacity)
	{
		// Effective growth rate k = 1.5
		const Index oldCapacity = currentCapacity;
		const Index additionalCapacity = oldCapacity / 2 + 1;
		const Index newCapacity = (maxCapacity() - oldCapacity >= additionalCapacity)
			? oldCapacity + additionalCapacity : maxCapacity();
		return newCapacity;
	}

private:
	struct StorageState
	{
		Index generation = nextGeneration(HandleType::INVALID_GENERATION);
		bool isFreed = true;
	};

	TAlignedMemoryUniquePtr<Item> m_storageMemory;
	std::vector<StorageState> m_storageStates;
	std::vector<Index> m_freeIndices;
};

}// end namespace ph::editor
