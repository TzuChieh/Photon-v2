#pragma once

#include "EditorCore/Storage/TItemPoolInterface.h"
#include "EditorCore/Storage/fwd.h"
#include "EditorCore/Storage/THandleDispatcher.h"
#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/assertion.h>
#include <Common/memory.h>
#include <Common/os.h>
#include <Common/math_basics.h>
#include <Common/exceptions.h>
#include <Common/config.h>

#include <cstddef>
#include <type_traits>
#include <limits>
#include <utility>
#include <memory>
#include <vector>
#include <numeric>
#include <new>
#include <algorithm>

namespace ph::editor
{

/*! @brief Item pool for simple types.
This item pool is designed to minimize execution time and memory footprint. As a result, some
operations are not possible compare to `TItemPool`, e.g., iterating over created items, clear, etc.
User is expected to keep track of the handles and use them to iterate the container the way they prefer.
When using manual handle management APIs, lost handles cannot be retrieved and those storage slots
are effectively leaked (pool dtor will still correctly free the memory though).

Note on the lack of `clear()` method: We are not tracking slot validity, so no chance to iterate
through valid/constructed items and remove them one by one. Simply clearing the whole pool and
return all handles to the dispatcher will face stale handle issues sooner or later, as pre-existing
valid handles (either the ones in the dispatcher or the ones tracked manually by the user) will
suddenly become invalid. A more complex API/policy may solve the problem, but the increased
complexity might not worth it, and such feature is not needed currently.

@tparam Dispatcher The dispatcher to use for this pool. Unlike `TItemPool`, trivial item pool does not
require the handle type to be related to `Item` in any way, i.e., the item type of handle does not
matter. This is useful for using the handle like an opaque pointer. Example: Specifying
`Item = std::string` and `Dispatcher = THandleDispatcher<TWeakHandle<int>>` is valid and
`TWeakHandle<int>` can be used for accessing the pool.
*/
template<typename Item, CHandleDispatcher Dispatcher = THandleDispatcher<TWeakHandle<Item>>>
class TTrivialItemPool : public TItemPoolInterface<Item, typename Dispatcher::HandleType>
{
	// For us to omit item validity tracking--we always start item lifetime after allocation
	static_assert(std::is_default_constructible_v<Item>,
		"Item must be default constructible.");

	// For us to grow the pool
	static_assert(std::is_move_constructible_v<Item>,
		"Item must be move constructible.");

	// For us to omit destruction
	static_assert(std::is_trivially_destructible_v<Item>,
		"Item must be trivially destructible.");

	// TODO: optimize item copy when we have std::is_implicit_lifetime

public:
	using HandleType = typename Dispatcher::HandleType;

private:
	using Index = typename HandleType::IndexType;
	using Generation = typename HandleType::GenerationType;
	using NonConstItem = std::remove_const_t<Item>;

public:
	TTrivialItemPool()
		: m_storageMemory()
		, m_generations()
		, m_dispatcher()
		, m_numItems(0)
	{}

	TTrivialItemPool(const TTrivialItemPool& other)
		requires std::is_copy_constructible_v<Item> && 
	             std::is_copy_constructible_v<Dispatcher>

		: m_storageMemory()
		, m_generations()
		, m_dispatcher(other.m_dispatcher)
		, m_numItems(other.m_numItems)
	{
		grow(other.capacity());

		// Copy all items (they are either created by user or default-constructed)
		for(Index i = 0; i < other.capacity(); ++i)
		{
			std::construct_at(
				getItemPtrDirectly(m_storageMemory, i),
				*getItemPtr(other.m_storageMemory, i));
		}

		// Copied after storage, since capacity is determined from `m_generations`
		m_generations = other.m_generations;
	}

	TTrivialItemPool(TTrivialItemPool&& other) noexcept
		: TTrivialItemPool()
	{
		swap(*this, other);
	}

	TTrivialItemPool& operator = (TTrivialItemPool rhs) noexcept
	{
		swap(*this, rhs);

		return *this;
	}

	~TTrivialItemPool() override = default;

	Item* accessItem(const HandleType& handle) override
	{
		return get(handle);
	}

	const Item* viewItem(const HandleType& handle) const override
	{
		return get(handle);
	}

	/*!
	Complexity: Amortized O(1). O(1) if `hasFreeSpace()` returns true.
	@return Handle of the added `item`. Cannot be discarded as the pool does not track item validity,
	hence lost handles cannot be recycled automatically.
	*/
	[[nodiscard]]
	HandleType add(Item item)
	{
		return createAt(dispatchOneHandle(), std::move(item));
	}

	/*! @brief Remove the item at the storage slot indicated by `handle`.
	Complexity: O(1).
	*/
	void remove(const HandleType& handle)
	{
		returnOneHandle(removeAt(handle));
	}

	/*! @brief Place `item` at the storage slot indicated by `handle`.
	Manual handle management API. Complexity: Amortized O(1). O(1) if `hasFreeSpace()` returns true.
	@return Handle of the created `item`. Same as the input `handle`.
	*/
	HandleType createAt(const HandleType& handle, Item item)
	{
		constexpr auto initialGeneration = HandleType::nextGeneration(HandleType::INVALID_GENERATION);

		const bool isEmpty = handle.isEmpty();
		const bool isInvalidOutOfBound = handle.getIndex() >= capacity() && handle.getGeneration() != initialGeneration;
		const bool isStale = handle.getIndex() < capacity() && !isFresh(handle);
		if(isEmpty || isInvalidOutOfBound || isStale)
		{
			throw_formatted<IllegalOperationException>(
				"creating trivial item with bad handle {}", handle);
		}

		// Potentially create new storage space
		const Index itemIdx = handle.getIndex();
		if(itemIdx >= capacity())
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
		PH_ASSERT_LT(itemIdx, m_generations.size());
		PH_ASSERT(isFresh(handle));

		// No need for storing the returned pointer nor using `std::launder()` on each use for most
		// cases (same object type with exactly the same storage location), see C++ standard [basic.life]
		// section 8 (https://timsong-cpp.github.io/cppwp/basic.life#8).
		std::construct_at(getItemPtrDirectly(m_storageMemory, itemIdx), std::move(item));

		++m_numItems;
		return handle;
	}

	/*! @brief Remove the item at the storage slot indicated by `handle`.
	Manual handle management API. Complexity: O(1).
	@return The handle for creating a new item on the storage slot that was indicated by `handle`.
	The returned handle should not be discarded and is expected to be returned to the pool later by
	calling `returnOneHandle()`.
	*/
	[[nodiscard]]
	HandleType removeAt(const HandleType& handle)
	{
		if(!isFresh(handle))
		{
			throw_formatted<IllegalOperationException>(
				"removing trivial item with stale handle {}", handle);
		}

		const Index itemIdx = handle.getIndex();
		PH_ASSERT_LT(itemIdx, capacity());

		// Calling dtor is not necessary as we are dealing with trivially destructible objects
		static_assert(std::is_trivially_destructible_v<Item>);

		// Instead, we clear it by default constructing a new instance
		std::construct_at(getItemPtr(m_storageMemory, itemIdx));

		const Generation newGeneration = HandleType::nextGeneration(handle.getGeneration());
		m_generations[itemIdx] = newGeneration;
		--m_numItems;
		return HandleType(handle.getIndex(), newGeneration);
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
	void returnOneHandle(const HandleType& handle)
	{
		// Note: call the dispatcher without touching internal states, as this method may be called
		// with a different policy (e.g., from a different thread, depending on the dispatcher used)
		m_dispatcher.returnOne(handle);
	}

	/*! @brief Get item by handle.
	Note that accessing items before construction is also allowed. If storage space is allocated for
	the target item, it will be initialized to the same value as a default-constructed item.
	Complexity: O(1).
	@return Pointer to the item. Null if item does not exist.
	*/
	Item* get(const HandleType& handle)
	{
		return isFresh(handle) ? getItemPtr(m_storageMemory, handle.getIndex()) : nullptr;
	}

	/*! @brief Get item by handle.
	Note that accessing items before construction is also allowed. If storage space is allocated for
	the target item, it will be initialized to the same value as a default-constructed item.
	Complexity: O(1).
	@return Pointer to the item. Null if item does not exist.
	*/
	const Item* get(const HandleType& handle) const
	{
		return isFresh(handle) ? getItemPtr(m_storageMemory, handle.getIndex()) : nullptr;
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
		PH_ASSERT_LE(m_generations.size(), maxCapacity());
		return static_cast<Index>(m_generations.size());
	}

	/*!
	@return Wether this pool contains any item.
	*/
	bool isEmpty() const
	{
		return numItems() == 0;
	}

	/*!
	@return Whether the handle is an up-to-date one for the underlying storage. Does not check for
	item validity.
	*/
	bool isFresh(const HandleType& handle) const
	{
		return handle.getIndex() < m_generations.size() &&
		       handle.getGeneration() == m_generations[handle.getIndex()];
	}

	/*! @brief Access item by index.
	@param index Index of the item. Valid in [0, `capacity()`).
	@return The item at `index`. Default constructed if it is not created already.
	*/
	///@{
	Item& operator [] (const std::size_t index)
	{
		PH_ASSERT_LT(index, m_generations.size());
		return *getItemPtr(m_storageMemory, index);
	}

	const Item& operator [] (const std::size_t index) const
	{
		PH_ASSERT_LT(index, m_generations.size());
		return *getItemPtr(m_storageMemory, index);
	}
	///@}

	static constexpr Index maxCapacity()
	{
		return std::numeric_limits<Index>::max();
	}

	friend void swap(TTrivialItemPool& first, TTrivialItemPool& second) noexcept
	{
		// Enable ADL
		using std::swap;

		swap(first.m_storageMemory, second.m_storageMemory);
		swap(first.m_generations, second.m_generations);
		swap(first.m_dispatcher, second.m_dispatcher);
		swap(first.m_numItems, second.m_numItems);
	}

private:
	void grow(const Index newCapacity)
	{
		const Index oldCapacity = capacity();
		PH_ASSERT_GT(newCapacity, oldCapacity);

		const auto requiredMemorySize = newCapacity * sizeof(NonConstItem);
		const auto alignmentSize = std::lcm(alignof(NonConstItem), os::get_L1_cache_line_size_in_bytes());
		const auto totalMemorySize = math::next_multiple(requiredMemorySize, alignmentSize);

		// Create new item storage and move items over

		TAlignedMemoryUniquePtr<NonConstItem> newStorageMemory =
			make_aligned_memory<NonConstItem>(totalMemorySize, alignmentSize);
		if(!newStorageMemory)
		{
			throw std::bad_alloc{};
		}

		// Copying/moving all items to new storage. No need (and no means) to check items from the
		// old storage are valid--they are either created by user or default-constructed when their
		// storage was first allocated.
		for(Index i = 0; i < oldCapacity; ++i)
		{
			std::construct_at(
				getItemPtrDirectly(newStorageMemory, i),
				std::move(*getItemPtr(m_storageMemory, i)));
		}

		// Set newly created storage space to default values, since accessing items before their
		// construction is explicitly stated to behave like they are default-constructed. Another reason
		// is that `Item` may not be an implicit-lifetime class, so C++20's Implicit Object Creation
		// cannot be relied upon (item lifetime may not begin unless placement new is used).
		std::uninitialized_default_construct_n(
			getItemPtrDirectly(newStorageMemory, oldCapacity),
			newCapacity - oldCapacity);

		// Extend generation records to cover new storage spaces
		constexpr auto initialGeneration = HandleType::nextGeneration(HandleType::INVALID_GENERATION);
		m_generations.resize(newCapacity, initialGeneration);

		// Finally, get rid of the old item storage
		m_storageMemory = std::move(newStorageMemory);
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
		// See [basic.life] section 8.3 (https://timsong-cpp.github.io/cppwp/basic.life#8.3).
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
	// LWG 3870 forbids `std::construct_at` to modify/create const objects. We store all items
	// as `NonConstItem` and rely on implicit casts to const if required. This seems to be a weaker
	// part of the standard and supporting const storage does not worth the risk of UB for now.
	// See `getItemPtr()` for more important things to know for supporting const items.
	TAlignedMemoryUniquePtr<NonConstItem> m_storageMemory;

	std::vector<Generation> m_generations;
	Dispatcher m_dispatcher;
	Index m_numItems;
};

}// end namespace ph::editor
