#pragma once

#include "EditorCore/Storage/TItemPoolInterface.h"
#include "EditorCore/Storage/THandleDispatcher.h"
#include "EditorCore/Storage/TWeakHandle.h"
#include "EditorCore/Storage/TStrongHandle.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Common/memory.h>
#include <Common/os.h>
#include <Common/compiler.h>
#include <Common/math_basics.h>
#include <Common/exceptions.h>
#include <Common/config.h>
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
@tparam Dispatcher The dispatcher to use for this pool. Item pool requires the handle type to be
"compatible" to `Item`, see the definition of `TCompatibleHandleType` for more detail.
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
	using NonConstItem = std::remove_const_t<Item>;

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

			return *getItemPtr(m_pool->m_storageMemory, m_currentIdx);
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

#if !PH_COMPILER_HAS_P2468R2
		// Inequality
		bool operator != (const TIterator& rhs) const
		{
			return !(*this == rhs);
		}
#endif

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

	TItemPool()
		: m_storageMemory()
		, m_storageStates()
		, m_dispatcher()
		, m_numItems(0)
	{}

	/*! @brief Copy items stored in `other` into this pool.
	Handles that were originally valid for `other` will also be valid for this pool.
	*/
	TItemPool(const TItemPool& other)
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

	TItemPool(TItemPool&& other) noexcept
		: TItemPool()
	{
		swap(*this, other);
	}

	TItemPool& operator = (TItemPool rhs) noexcept
	{
		swap(*this, rhs);

		return *this;
	}

	~TItemPool() override
	{
		clear();

		// All items should be removed at the end
		PH_ASSERT_EQ(m_numItems, 0);
	}

	ItemInterface* accessItem(const HandleType& handle) override
	{
		return get(handle);
	}

	const ItemInterface* viewItem(const HandleType& handle) const override
	{
		return get(handle);
	}

	/*!
	Will not invalidate iterators. Complexity: Amortized O(1). O(1) if `hasFreeSpace()` returns true.
	@return Handle of the added `item`.
	*/
	HandleType add(Item item)
	{
		return createAt(dispatchOneHandle(), std::move(item));
	}

	/*! @brief Remove the item at the storage slot indicated by `handle`.
	Will not invalidate iterators. Complexity: O(1).
	*/
	template<typename ItemType>
	void remove(const TCompatibleHandleType<ItemType>& handle)
	{
		returnOneHandle(removeAt(handle));
	}

	/*! @brief Place `item` at the storage slot indicated by `handle`.
	Manual handle management API. Will not invalidate iterators. Complexity: Amortized O(1).
	O(1) if `hasFreeSpace()` returns true.
	@return Handle of the created `item`. Equivalent to the input `handle`.
	*/
	template<typename ItemType>
	HandleType createAt(const TCompatibleHandleType<ItemType>& handle, Item item)
	{
		constexpr auto initialGeneration = HandleType::nextGeneration(HandleType::INVALID_GENERATION);

		const bool isEmpty = handle.isEmpty();
		const bool isInvalidOutOfBound = handle.getIndex() >= capacity() && handle.getGeneration() != initialGeneration;
		const bool isStale = handle.getIndex() < capacity() && !isFresh(handle);
		if(isEmpty || isInvalidOutOfBound || isStale)
		{
			throw_formatted<IllegalOperationException>(
				"creating item with bad handle {}", handle);
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
				"attempting to create item at an occupied slot (handle: {})", handle);
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
	HandleType removeAt(const TCompatibleHandleType<ItemType>& handle)
	{
		if(!isFresh(handle))
		{
			throw_formatted<IllegalOperationException>(
				"removing item with stale handle {}", handle);
		}

		const Index itemIdx = handle.getIndex();

		// Generally should not happen: the generation counter increases on each item removal, using
		// the same handle to call this method again will result in `isFresh()` being false which
		// will throw. If this fails, could be generation collision, use after removal, remove before
		// creation, or bad handles (from wrong pool).
		if(m_storageStates[itemIdx].isFreed)
		{
			throw_formatted<IllegalOperationException>(
				"attempting to remove item at an emptied slot (handle: {})", handle);
		}

		removeItemAtIndex(itemIdx);
		return getHandleByIndex(itemIdx);
	}

	/*!
	Manual handle management API.
	*/
	[[nodiscard]]
	HandleType dispatchOneHandle()
	{
		// Note: call the dispatcher without touching internal states, as this method may be called
		// with a different policy (e.g., from a different thread, depending on the dispatcher used)
		return m_dispatcher.dispatchOne();
	}

	/*!
	Manual handle management API.
	*/
	template<typename ItemType>
	void returnOneHandle(const TCompatibleHandleType<ItemType>& handle)
	{
		HandleType nativeHandle(handle.getIndex(), handle.getGeneration());

		// Note: call the dispatcher without touching internal states, as this method may be called
		// with a different policy (e.g., from a different thread, depending on the dispatcher used)
		m_dispatcher.returnOne(nativeHandle);
	}

	void clear()
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
	ItemType* get(const TCompatibleHandleType<ItemType>& handle)
	{
		return isFresh(handle) && !m_storageStates[handle.getIndex()].isFreed
			? getItemPtr(m_storageMemory, handle.getIndex())
			: nullptr;
	}

	/*! @brief Get item by handle.
	Complexity: O(1).
	@return Pointer to the item. Null if item does not exist. For a valid, dispatched handle with
	uninitialized item (not yet created), null will also be returned.
	*/
	template<typename ItemType>
	const ItemType* get(const TCompatibleHandleType<ItemType>& handle) const
	{
		return isFresh(handle) && !m_storageStates[handle.getIndex()].isFreed
			? getItemPtr(m_storageMemory, handle.getIndex())
			: nullptr;
	}

	template<typename ItemType>
	auto getStrong(const TCompatibleHandleType<ItemType>& handle)
	-> TStrongHandle<ItemInterface, Index, Generation>
	{
		using StrongHandle = TStrongHandle<ItemInterface, Index, Generation>;
		using EmbeddedWeakHandle = typename StrongHandle::WeakHandleType;

		static_assert(std::convertible_to<decltype(handle), EmbeddedWeakHandle>,
			"Input handle type cannot be converted to a strong handle.");

		return StrongHandle(EmbeddedWeakHandle(handle), this);
	}

	Index numItems() const
	{
		PH_ASSERT_LE(m_numItems, capacity());
		return m_numItems;
	}

	Index numFreeSpace() const
	{
		PH_ASSERT_LE(m_numItems, capacity());
		return capacity() - m_numItems;
	}

	Index capacity() const
	{
		PH_ASSERT_LE(m_storageStates.size(), maxCapacity());
		return static_cast<Index>(m_storageStates.size());
	}

	/*!
	@return Wether this pool contains any item.
	*/
	bool isEmpty() const
	{
		return numItems() == 0;
	}

	bool hasFreeSpace() const
	{
		return numFreeSpace() > 0;
	}

	/*!
	@return Whether the handle is an up-to-date one for the underlying storage. Does not check for
	item validity.
	*/
	template<typename ItemType>
	bool isFresh(const TCompatibleHandleType<ItemType>& handle) const
	{
		return handle.getIndex() < m_storageStates.size() &&
		       handle.getGeneration() == m_storageStates[handle.getIndex()].generation;
	}

	/*! @brief Iterators for all created items in the pool.
	*/
	///@{
	IteratorType begin()
	{
		return IteratorType(this, nextItemBeginIndex(0));
	}

	IteratorType end()
	{
		return IteratorType(this, capacity());
	}

	ConstIteratorType cbegin()
	{
		return IteratorType(this, nextItemBeginIndex(0));
	}

	ConstIteratorType cend()
	{
		return IteratorType(this, capacity());
	}
	///@}

	static constexpr Index maxCapacity()
	{
		return std::numeric_limits<Index>::max();
	}

	friend void swap(TItemPool& first, TItemPool& second) noexcept
	{
		// Enable ADL
		using std::swap;

		swap(first.m_storageMemory, second.m_storageMemory);
		swap(first.m_storageStates, second.m_storageStates);
		swap(first.m_dispatcher, second.m_dispatcher);
		swap(first.m_numItems, second.m_numItems);
	}

private:
	void createItemAtIndex(const Index itemIdx, Item item)
	{
		PH_ASSERT_LT(itemIdx, m_storageStates.size());
		PH_ASSERT(m_storageStates[itemIdx].isFreed);

		// `Item` was manually destroyed. No need for storing the returned pointer nor using
		// `std::launder()` on each use for most cases (same object type with exactly the same storage
		// location), see C++ standard [basic.life] section 8 (https://timsong-cpp.github.io/cppwp/basic.life#8).
		std::construct_at(getItemPtrDirectly(m_storageMemory, itemIdx), std::move(item));

		m_storageStates[itemIdx].isFreed = false;
		++m_numItems;
	}

	void removeItemAtIndex(const Index itemIdx)
	{
		PH_ASSERT_LT(itemIdx, capacity());
		PH_ASSERT(!m_storageStates[itemIdx].isFreed);

		std::destroy_at(getItemPtr(m_storageMemory, itemIdx));
		
		m_storageStates[itemIdx].isFreed = true;
		m_storageStates[itemIdx].generation = HandleType::nextGeneration(m_storageStates[itemIdx].generation);
		--m_numItems;
	}

	HandleType getHandleByIndex(const Index itemIdx) const
	{
		return HandleType(itemIdx, m_storageStates[itemIdx].generation);
	}

	void grow(const Index newCapacity)
	{
		const Index oldCapacity = capacity();
		PH_ASSERT_GT(newCapacity, oldCapacity);

		const auto requiredMemorySize = newCapacity * sizeof(Item);
		const auto alignmentSize = std::lcm(alignof(Item), os::get_L1_cache_line_size_in_bytes());
		const auto totalMemorySize = math::next_multiple(requiredMemorySize, alignmentSize);

		// Create new item storage and move items over

		TAlignedMemoryUniquePtr<NonConstItem> newStorageMemory =
			make_aligned_memory<NonConstItem>(totalMemorySize, alignmentSize);
		if(!newStorageMemory)
		{
			throw std::bad_alloc{};
		}

		forEachItem(
			[&newStorageMemory](Item* oldItem, Index idx)
			{
				std::construct_at(getItemPtrDirectly(newStorageMemory, idx), std::move(*oldItem));
				std::destroy_at(oldItem);
			});

		// Extend storage states to cover new storage spaces
		m_storageStates.resize(newCapacity, StorageState{});

		// Finally, get rid of the old item storage
		m_storageMemory = std::move(newStorageMemory);
	}

	template<typename PerItemStorageOp>
	void forEachItemStorage(PerItemStorageOp op)
	{
		for(Index itemIdx = 0; itemIdx < capacity(); ++itemIdx)
		{
			Item* item = m_storageStates[itemIdx].isFreed
				? getItemPtrDirectly(m_storageMemory, itemIdx)
				: getItemPtr(m_storageMemory, itemIdx);
			op(item, itemIdx);
		}
	}

	template<typename PerItemStorageOp>
	void forEachItemStorage(PerItemStorageOp op) const
	{
		for(Index itemIdx = 0; itemIdx < capacity(); ++itemIdx)
		{
			const Item* item = m_storageStates[itemIdx].isFreed
				? getItemPtrDirectly(m_storageMemory, itemIdx)
				: getItemPtr(m_storageMemory, itemIdx);
			op(item, itemIdx);
		}
	}

	template<typename PerItemOp>
	void forEachItem(PerItemOp op)
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
	void forEachItem(PerItemOp op) const
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

	Index nextItemBeginIndex(const Index beginIdx) const
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

	Index previousItemEndIndex(const Index endIdx) const
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

	/*! @brief Get pointer to item given its storage.
	This method helps to automatically do pointer laundering if required. Note that the pointed-to item
	must be within its lifetime, otherwise it is UB.
	*/
	static auto getItemPtr(
		const TAlignedMemoryUniquePtr<NonConstItem>& storage,
		const std::size_t index)
	-> NonConstItem*
	{
		// If `Item` is const qualified, laundering is required to prevent aggressive constant folding.
		// See [basic.life] section 8.3 (https://timsong-cpp.github.io/cppwp/basic.life#8.3)
		//                   vvv currently not required as we are storing `NonConstItem`
		if constexpr(/* std::is_const_v<Item> || */ PH_STRICT_OBJECT_LIFETIME)
		{
			// UB if pointed-to object not within its lifetime
			return std::launder(getItemPtrDirectly(storage, index));
		}
		// We do not need to launder even if `Item` contains const or reference members. See
		// https://stackoverflow.com/questions/62642542/were-all-implementations-of-stdvector-non-portable-before-stdlaunder
		else
		{
			return getItemPtrDirectly(storage, index);
		}
	}

	static auto getItemPtrDirectly(
		const TAlignedMemoryUniquePtr<NonConstItem>& storage,
		const std::size_t index)
	-> NonConstItem*
	{
		return storage.get() + index;
	}

	static constexpr Index nextCapacity(const Index currentCapacity)
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
		uint8 isFreed : 1 = true;

		// Can support more tags, e.g., isDisabled for disabling an item slot permanently
		// so generation collision will never happen.
	};

	// LWG 3870 forbids `std::construct_at` to modify/create const objects. We store all items
	// as `NonConstItem` and rely on implicit casts to const if required. This seems to be a weaker
	// part of the standard and supporting const storage does not worth the risk of UB for now.
	// See `getItemPtr()` for more important things to know for supporting const items.
	TAlignedMemoryUniquePtr<NonConstItem> m_storageMemory;

	std::vector<StorageState> m_storageStates;
	Dispatcher m_dispatcher;
	Index m_numItems;
};

}// end namespace ph::editor
