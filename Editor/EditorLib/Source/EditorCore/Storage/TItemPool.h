#pragma once

#include "EditorCore/Storage/TItemPoolInterface.h"
#include "EditorCore/Storage/THandleDispatcher.h"
#include "EditorCore/Storage/TWeakHandle.h"
#include "EditorCore/Storage/TStrongHandle.h"

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

/*! @brief A general item pool.
@tparam Item Type of the stored datum. Can be any class or primitive type. Must be move constructible.
*/
template<
	typename Item, 
	CHandleDispatcher Dispatcher = THandleDispatcher<TWeakHandle<Item>>, 
	typename ItemInterface = Item>
class TItemPool : public TItemPoolInterface<ItemInterface, typename Dispatcher::HandleType>
{
	static_assert(std::is_move_constructible_v<Item>,
		"Item must be move constructible.");

public:
	using HandleType = typename Dispatcher::HandleType;

private:
	using Base = TItemPoolInterface<ItemInterface, HandleType>;
	using Index = typename HandleType::IndexType;
	using Generation = typename HandleType::GenerationType;

public:
	/*! If `Item` is a polymorphic type, then `Item` itself along with all its bases can form a
	valid handle type. These compatible handles can be used for accessing items stored in the pool
	(items obtained have the same item type indicated by the handle, for type safety).
	*/
	template<typename ItemType> requires std::is_scalar_v<Item> || CBase<ItemType, Item>
	using TCompatibleHandleType = TWeakHandle<ItemType, Index, Generation>;

	static_assert(std::is_same_v<typename Base::HandleType, TCompatibleHandleType<Item>>,
		"TItemPoolInterface is not using a compatible HandleType.");

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
		TIterator() = default;

		TIterator(PoolType* const pool, Index currentIdx)
			: m_pool(pool)
			, m_currentIdx(currentIdx)
		{}

		// Dereferenceable
		reference operator * () const
		{
			PH_ASSERT(m_pool);
			PH_ASSERT_LT(m_currentIdx, m_pool->m_storageStates.size());
			PH_ASSERT(!m_pool->m_storageStates[m_currentIdx].isFreed);

			return m_pool->m_storageMemory.get()[m_currentIdx];
		}

		// Pre-incrementable
		TIterator& operator ++ ()
		{
			PH_ASSERT(m_pool);

			++m_currentIdx;
			m_currentIdx = m_pool->nextItemBeginIndex(m_currentIdx);
			return *this;
		}

		// Post-incrementable
		TIterator operator ++ (int)
		{
			TIterator current = *this;
			++(*this);
			return current;
		}

		// Pre-decrementable
		TIterator& operator -- ()
		{
			PH_ASSERT(m_pool);

			// Using current index as end index is equivalent to decrement by 1
			m_currentIdx = m_pool->previousItemEndIndex(m_currentIdx);
			PH_ASSERT_GT(m_currentIdx, 0);// If this method can be called, there must exist a previous
			--m_currentIdx;               // item and `m_currentIdx` is impossible to reach 0 since
			return *this;                 // it is used as an (exclusive) end index here.
		}

		// Post-decrementable
		TIterator operator -- (int)
		{
			TIterator current = *this;
			--(*this);
			return current;
		}

		// Equality
		bool operator == (const TIterator& rhs) const
		{
			return m_currentIdx == rhs.m_currentIdx && m_pool == rhs.m_pool;
		}

		// Inequality
		bool operator != (const TIterator& rhs) const
		{
			return !(*this == rhs);
		}

		/*!
		@return Fresh handle of the current valid item.
		*/
		HandleType getHandle() const
		{
			PH_ASSERT(m_pool);
			PH_ASSERT(!m_pool->m_storageStates[m_currentIdx].isFreed);

			return m_pool->getHandleByIndex(m_currentIdx);
		}

	private:
		PoolType* m_pool = nullptr;

		// No hard referencing on the current item, so modification to the pool will not invalidate
		// this iterator.
		Index m_currentIdx = HandleType::INVALID_INDEX;
	};

public:
	using IteratorType = TIterator<false>;
	using ConstIteratorType = TIterator<true>;

	inline TItemPool()
		: m_storageMemory()
		, m_storageStates()
		, m_dispatcher()
		, m_numItems(0)
	{}

	/*! @brief Copy items stored in `other` into this pool.
	Handles that were originally valid for `other` will also be valid for this pool.
	*/
	inline TItemPool(const TItemPool& other)
		requires std::is_copy_constructible_v<Item> &&
		         std::is_copy_constructible_v<Dispatcher>

		: m_storageMemory()
		, m_storageStates()
		, m_dispatcher(other.m_dispatcher)
		, m_numItems(0)
	{
		grow(other.capacity());

		other.forEachItemStorage(
			[this, &other](const Item* otherItem, Index idx)
			{
				// Copy generation state, so handles from `other` will also work here
				m_storageStates[idx].generation = other.m_storageStates[idx].generation;

				if(!other.m_storageStates[idx].isFreed)
				{
					createItemAtIndex(idx, *otherItem);
				}
			});

		PH_ASSERT_EQ(m_numItems, other.m_numItems);
	}

	inline TItemPool(TItemPool&& other) noexcept
		: TItemPool()
	{
		swap(*this, other);
	}

	inline TItemPool& operator = (TItemPool rhs) noexcept
	{
		swap(*this, rhs);

		return *this;
	}

	inline ~TItemPool() override
	{
		clear();

		// All items should be removed at the end
		PH_ASSERT_EQ(m_numItems, 0);
	}

	inline ItemInterface* accessItem(const HandleType& handle) override
	{
		return get(handle);
	}

	inline const ItemInterface* viewItem(const HandleType& handle) const override
	{
		return get(handle);
	}

	/*!
	Will not invalidate iterators. Complexity: Amortized O(1). O(1) if `hasFreeSpace()` returns true.
	@return Handle of the added `item`.
	*/
	inline HandleType add(Item item)
	{
		return createAt(dispatchOneHandle(), std::move(item));
	}

	/*! @brief Remove the item at the storage slot indicated by `handle`.
	Will not invalidate iterators. Complexity: O(1).
	*/
	template<typename ItemType>
	inline void remove(const TCompatibleHandleType<ItemType>& handle)
	{
		returnOneHandle(removeAt(handle));
	}

	/*! @brief Place `item` at the storage slot indicated by `handle`.
	Manual handle management API. Will not invalidate iterators. Complexity: Amortized O(1).
	O(1) if `hasFreeSpace()` returns true.
	@return Handle of the created `item`. Equivalent to the input `handle`.
	*/
	template<typename ItemType>
	inline HandleType createAt(const TCompatibleHandleType<ItemType>& handle, Item item)
	{
		constexpr auto initialGeneration = HandleType::nextGeneration(HandleType::INVALID_GENERATION);

		const bool isEmpty = handle.isEmpty();
		const bool isInvalidOutOfBound = handle.getIndex() >= capacity() && handle.getGeneration() != initialGeneration;
		const bool isStale = handle.getIndex() < capacity() && !isFresh(handle);
		if(isEmpty || isInvalidOutOfBound || isStale)
		{
			throw_formatted<IllegalOperationException>(
				"creating item with bad handle ({})",
				handle.toString());
		}

		// Potentially create new storage space
		const Index itemIdx = handle.getIndex();
		if(!hasFreeSpace())
		{
			if(capacity() == maxCapacity())
			{
				throw_formatted<OverflowException>(
					"Storage size will exceed the maximum amount Index type can hold (max={})",
					maxCapacity());
			}

			PH_ASSERT_LT(itemIdx, maxCapacity());
			const Index newCapacity = std::max(nextCapacity(capacity()), itemIdx + 1);
			grow(newCapacity);
		}

		// At this point, storage size must have been grown to cover `itemIdx`
		PH_ASSERT_LT(itemIdx, m_storageStates.size());
		PH_ASSERT(isFresh(handle));

		if(!m_storageStates[itemIdx].isFreed)
		{
			throw_formatted<IllegalOperationException>(
				"attempting to create item at an occupied slot (handle: {})",
				handle.toString());
		}

		createItemAtIndex(itemIdx, std::move(item));
		return getHandleByIndex(itemIdx);
	}

	/*! @brief Remove the item at the storage slot indicated by `handle`.
	Manual handle management API. Will not invalidate iterators. Complexity: O(1).
	@return The handle for creating a new item on the storage slot that was indicated by `handle`.
	The returned handle should not be discarded and is expected to be returned to the pool later by
	calling `returnOneHandle()`.
	*/
	template<typename ItemType>
	[[nodiscard]]
	inline HandleType removeAt(const TCompatibleHandleType<ItemType>& handle)
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
		// will throw. If this fails, could be generation collision, use after removal, remove before
		// creation, or bad handles (from wrong pool).
		if(m_storageStates[itemIdx].isFreed)
		{
			throw_formatted<IllegalOperationException>(
				"attempting to remove item at an emptied slot (handle: {})",
				handle.toString());
		}

		removeItemAtIndex(itemIdx);
		return getHandleByIndex(itemIdx);
	}

	/*!
	Manual handle management API.
	*/
	[[nodiscard]]
	inline HandleType dispatchOneHandle()
	{
		// Note: call the dispatcher without touching internal states, as this method may be called
		// with a different policy (e.g., from a different thread, depending on the dispatcher used)
		return m_dispatcher.dispatchOne();
	}

	/*!
	Manual handle management API.
	*/
	template<typename ItemType>
	inline void returnOneHandle(const TCompatibleHandleType<ItemType>& handle)
	{
		HandleType nativeHandle(handle.getIndex(), handle.getGeneration());

		// Note: call the dispatcher without touching internal states, as this method may be called
		// with a different policy (e.g., from a different thread, depending on the dispatcher used)
		m_dispatcher.returnOne(nativeHandle);
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
				removeItemAtIndex(idx);
				returnOneHandle(getHandleByIndex(idx));
			});
	}

	/*! @brief Get item by handle.
	Complexity: O(1).
	@return Pointer to the item. Null if item does not exist. For a valid, dispatched handle with
	uninitialized item (not yet created), null will also be returned.
	*/
	template<typename ItemType>
	inline ItemType* get(const TCompatibleHandleType<ItemType>& handle)
	{
		return isFresh(handle) && !m_storageStates[handle.getIndex()].isFreed
			? (m_storageMemory.get() + handle.getIndex())
			: nullptr;
	}

	/*! @brief Get item by handle.
	Complexity: O(1).
	@return Pointer to the item. Null if item does not exist. For a valid, dispatched handle with
	uninitialized item (not yet created), null will also be returned.
	*/
	template<typename ItemType>
	inline const ItemType* get(const TCompatibleHandleType<ItemType>& handle) const
	{
		return isFresh(handle) && !m_storageStates[handle.getIndex()].isFreed
			? (m_storageMemory.get() + handle.getIndex())
			: nullptr;
	}

	template<typename ItemType>
	inline auto getStrong(const TCompatibleHandleType<ItemType>& handle)
	-> TStrongHandle<ItemInterface, Index, Generation>
	{
		using StrongHandle = TStrongHandle<ItemInterface, Index, Generation>;
		using EmbeddedWeakHandle = typename StrongHandle::WeakHandleType;

		static_assert(std::convertible_to<decltype(handle), EmbeddedWeakHandle>,
			"Input handle type cannot be converted to a strong handle.");

		return StrongHandle(EmbeddedWeakHandle(handle), this);
	}

	inline Index numItems() const
	{
		PH_ASSERT_LE(m_numItems, capacity());
		return m_numItems;
	}

	inline Index numFreeSpace() const
	{
		PH_ASSERT_LE(m_numItems, capacity());
		return capacity() - m_numItems;
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

	/*!
	@return Whether the handle is an up-to-date one for the underlying storage. Does not check for
	item validity.
	*/
	template<typename ItemType>
	inline bool isFresh(const TCompatibleHandleType<ItemType>& handle) const
	{
		return handle.getIndex() < m_storageStates.size() &&
		       handle.getGeneration() == m_storageStates[handle.getIndex()].generation;
	}

	/*! @brief Iterators for all created items in the pool.
	*/
	///@{
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
	///@}

	inline static constexpr Index maxCapacity()
	{
		return std::numeric_limits<Index>::max();
	}

	inline friend void swap(TItemPool& first, TItemPool& second) noexcept
	{
		// Enable ADL
		using std::swap;

		swap(first.m_storageMemory, second.m_storageMemory);
		swap(first.m_storageStates, second.m_storageStates);
		swap(first.m_dispatcher, second.m_dispatcher);
		swap(first.m_numItems, second.m_numItems);
	}

private:
	inline void createItemAtIndex(const Index itemIdx, Item item)
	{
		PH_ASSERT_LT(itemIdx, m_storageStates.size());
		PH_ASSERT(m_storageStates[itemIdx].isFreed);

		// `Item` was manually destroyed. No need for storing the returned pointer nor using
		// `std::launder()` on each use (same object type with exactly the same storage location), 
		// see C++ standard [basic.life] section 8 (https://timsong-cpp.github.io/cppwp/n4659/basic.life#8).
		std::construct_at(m_storageMemory.get() + itemIdx, std::move(item));

		m_storageStates[itemIdx].isFreed = false;
		++m_numItems;
	}

	inline void removeItemAtIndex(const Index itemIdx)
	{
		PH_ASSERT_LT(itemIdx, capacity());
		PH_ASSERT(!m_storageStates[itemIdx].isFreed);

		std::destroy_at(m_storageMemory.get() + itemIdx);
		
		m_storageStates[itemIdx].isFreed = true;
		m_storageStates[itemIdx].generation = HandleType::nextGeneration(m_storageStates[itemIdx].generation);
		--m_numItems;
	}

	inline HandleType getHandleByIndex(const Index itemIdx) const
	{
		return HandleType(itemIdx, m_storageStates[itemIdx].generation);
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
		Generation generation = HandleType::nextGeneration(HandleType::INVALID_GENERATION);
		bool isFreed = true;
	};

	TAlignedMemoryUniquePtr<Item> m_storageMemory;
	std::vector<StorageState> m_storageStates;
	Dispatcher m_dispatcher;
	Index m_numItems;
};

}// end namespace ph::editor
