#pragma once

#include "EditorCore/Storage/TItemPoolInterface.h"
#include "EditorCore/Storage/fwd.h"

#include <Common/assertion.h>
#include <Common/memory.h>
#include <Common/os.h>
#include <Common/math_basics.h>
#include <Utility/exception.h>

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

/*! @brief Item pool for trivially-copyable types.
*/
template<typename Item, CHandleDispatcher Dispatcher>
class TTrivialItemPool : public TItemPoolInterface<Item, typename Dispatcher::HandleType>
{
	static_assert(std::is_default_constructible_v<Item>,
		"A trivial item must be default constructible.");
	static_assert(std::is_trivially_copyable_v<Item>,
		"A trivial item must be trivially copyable.");

public:
	using HandleType = typename Dispatcher::HandleType;

private:
	using Index = typename HandleType::IndexType;
	using Generation = typename HandleType::GenerationType;

public:
	inline TTrivialItemPool()
		: m_storageMemory()
		, m_generations()
		, m_dispatcher()
		, m_numItems(0)
	{}

	inline TTrivialItemPool(const TTrivialItemPool& other) requires std::is_copy_constructible_v<Dispatcher>
		: m_storageMemory()// copied in ctor body
		, m_generations()// copied in ctor body
		, m_dispatcher(other.m_dispatcher)
		, m_numItems(other.m_numItems)
	{
		grow(other.capacity());

		// Copying the memory starts new object lifetime. Behavior is finally defined since C++20
		// (see C++20's Implicit Object Creation)
		std::copy(
			other.m_storageMemory.get(),
			other.m_storageMemory.get() + other.capacity(),
			m_storageMemory.get());

		// Copied after storage as capacity is determined from `m_generations`
		m_generations = other.m_generations;
	}

	inline TTrivialItemPool(TTrivialItemPool&& other) noexcept = default;

	inline TTrivialItemPool& operator = (TTrivialItemPool rhs)
	{
		using std::swap;

		swap(*this, rhs);

		return *this;
	}

	inline ~TTrivialItemPool() override
	{
		clear();

		// All items should be removed at the end
		PH_ASSERT_EQ(m_numItems, 0);
	}

	inline Item* accessItem(const HandleType& handle) override
	{
		return get(handle);
	}

	inline const Item* viewItem(const HandleType& handle) const override
	{
		return get(handle);
	}

	/*!
	Complexity: Amortized O(1). O(1) if `hasFreeSpace()` returns true.
	@return The handle of the added `item`.
	*/
	inline HandleType add(Item item)
	{
		return createAt(dispatchOneHandle(), std::move(item));
	}

	/*! @brief Remove the item at the storage slot indicated by `handle`.
	Complexity: O(1).
	*/
	inline void remove(const HandleType& handle)
	{
		returnOneHandle(removeAt(handle));
	}

	/*! @brief Place `item` at the storage slot indicated by `handle`.
	Complexity: Amortized O(1). O(1) if `hasFreeSpace()` returns true.
	@return The handle of the created `item`. Same as the input `handle`.
	*/
	inline HandleType createAt(const HandleType& handle, Item item)
	{
		constexpr auto initialGeneration = HandleType::nextGeneration(HandleType::INVALID_GENERATION);

		const bool isEmpty = handle.isEmpty();
		const bool isInvalidOutOfBound = handle.getIndex() >= capacity() && handle.getGeneration() != initialGeneration;
		const bool isStale = handle.getIndex() < capacity() && !isFresh(handle);
		if(isEmpty || isInvalidOutOfBound || isStale)
		{
			throw_formatted<IllegalOperationException>(
				"creating trivial item with bad handle ({})",
				handle.toString());
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

		// `Item` was manually destroyed. No need for storing the returned pointer nor using
		// `std::launder()` on each use (same object type with exactly the same storage location), 
		// see C++ standard [basic.life] section 8 (https://timsong-cpp.github.io/cppwp/n4659/basic.life#8).
		std::construct_at(m_storageMemory.get() + itemIdx, std::move(item));

		++m_numItems;
		return handle;
	}

	/*! @brief Remove the item at the storage slot indicated by `handle`.
	Complexity: O(1).
	@return The handle for creating a new item on the storage slot that was indicated by `handle`.
	The returned handle must not be discarded as there are no defined means to retrieve the handle
	for a free storage slot.
	*/
	[[nodiscard]]
	inline HandleType removeAt(const HandleType& handle)
	{
		if(!isFresh(handle))
		{
			throw_formatted<IllegalOperationException>(
				"removing trivial item with stale handle ({})",
				handle.toString());
		}

		const Index itemIdx = handle.getIndex();
		PH_ASSERT_LT(itemIdx, capacity());

		// This is not necessary as we are dealing with trivially destructible objects,
		// just to be safe and consistent
		std::destroy_at(m_storageMemory.get() + itemIdx);

		const Generation newGeneration = HandleType::nextGeneration(handle.getGeneration());
		m_generations[itemIdx] = newGeneration;
		--m_numObjs;
		return HandleType(handle.getIndex(), newGeneration);
	}

	inline HandleType dispatchOneHandle()
	{
		// Note: directly call the dispatcher, as this method may be called with a different policy
		// (e.g., from a different thread, depending on the dispatcher used)
		return m_dispatcher.dispatchOne();
	}

	inline void returnOneHandle(const HandleType& handle)
	{
		// Note: directly call the dispatcher, as this method may be called with a different policy
		// (e.g., from a different thread, depending on the dispatcher used)
		m_dispatcher.returnOne(handle);
	}

	inline void clear()
	{
		// We are not tracking slot validity, no chance & no need to destruct objects manually as
		// we are dealing with trivially destructible objects
		m_numItems = 0;
	}

	/*!
	Complexity: O(1).
	*/
	inline Item* get(const HandleType& handle)
	{
		return isFresh(handle) ? (m_storageMemory.get() + handle.getIndex()) : nullptr;
	}

	/*!
	Complexity: O(1).
	*/
	inline const Item* get(const HandleType& handle) const
	{
		return isFresh(handle) ? (m_storageMemory.get() + handle.getIndex()) : nullptr;
	}

	inline Index numItems() const
	{
		PH_ASSERT_LE(m_numItems, capacity());
		return m_numItems;
	}

	inline Index numFreeSpace() const
	{
		PH_ASSERT_LE(numItems(), capacity());
		return capacity() - numItems();
	}

	inline Index capacity() const
	{
		PH_ASSERT_LE(m_generations.size(), maxCapacity());
		return static_cast<Index>(m_generations.size());
	}

	/*!
	@return Wether this pool contains any item.
	*/
	inline bool isEmpty() const
	{
		return numItems() == 0;
	}

	inline bool isFresh(const HandleType& handle) const
	{
		return handle.getIndex() < m_generations.size() &&
		       handle.getGeneration() == m_generations[handle.getIndex()];
	}

	inline static constexpr Index maxCapacity()
	{
		return std::numeric_limits<Index>::max();
	}

	inline friend void swap(TTrivialItemPool& first, TTrivialItemPool& second)
	{
		// Enable ADL
		using std::swap;

		swap(first.m_storageMemory, second.m_storageMemory);
		swap(first.m_generations, second.m_generations);
		swap(first.m_dispatcher, second.m_dispatcher);
		swap(first.m_numItems, second.m_numItems);
	}

private:
	inline void grow(const Index newCapacity)
	{
		const Index oldCapacity = capacity();
		PH_ASSERT_GT(newCapacity, oldCapacity);

		const auto requiredMemorySize = newCapacity * sizeof(Item);
		const auto alignmentSize = std::lcm(alignof(Item), os::get_L1_cache_line_size_in_bytes());
		const auto totalMemorySize = math::next_multiple(requiredMemorySize, alignmentSize);

		// Create new item storage and move items over

		TAlignedMemoryUniquePtr<Item> newStorageMemory = 
			make_aligned_memory<Item>(totalMemorySize, alignmentSize);
		if(!newStorageMemory)
		{
			throw std::bad_alloc();
		}

		// Copying the memory starts new object lifetime. Behavior is finally defined since C++20
		// (see C++20's Implicit Object Creation)
		std::copy(
			m_storageMemory.get(), 
			m_storageMemory.get() + oldCapacity,
			newStorageMemory.get());

		// Extend generation records to cover new storage spaces
		constexpr auto initialGeneration = HandleType::nextGeneration(HandleType::INVALID_GENERATION);
		m_generations.resize(newCapacity, initialGeneration);

		// Finally, get rid of the old item storage
		m_storageMemory = std::move(newStorageMemory);
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
	TAlignedMemoryUniquePtr<Item> m_storageMemory;
	std::vector<Generation> m_generations;
	Dispatcher m_dispatcher;
	Index m_numItems;
};

}// end namespace ph::editor
