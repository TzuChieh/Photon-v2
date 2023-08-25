#pragma once

#include "EditorCore/Storage/TItemPoolInterface.h"
#include "EditorCore/Storage/TWeakHandle.h"

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

/*! @brief Graphics object pool.
*/
template<typename Object, CWeakHandle Handle = TWeakHandle<Object>>
class TGraphicsObjectPool : public TItemPoolInterface<Object, Handle>
{
	static_assert(std::is_default_constructible_v<Object>,
		"A graphics object must be default constructible.");
	static_assert(std::is_trivially_copyable_v<Object>,
		"A graphics object must be trivially copyable.");

	using Index = typename Handle::IndexType;
	using Generation = typename Handle::GenerationType;

public:
	inline TGraphicsObjectPool()
		: m_storageMemory()
		, m_generations()
		, m_numObjs(0)
	{}

	inline TGraphicsObjectPool(const TGraphicsObjectPool& other)
		: TGraphicsObjectPool()
	{
		grow(other.capacity());

		// Copying the memory starts new object lifetime. Behavior is finally defined since C++20
		// (see C++20's Implicit Object Creation)
		std::copy(
			other.m_storageMemory.get(),
			other.m_storageMemory.get() + other.capacity(),
			m_storageMemory.get());

		m_generations = other.m_generations;
		m_numObjs = other.m_numObjs;
	}

	inline TGraphicsObjectPool(TGraphicsObjectPool&& other) noexcept = default;

	inline TGraphicsObjectPool& operator = (TGraphicsObjectPool rhs)
	{
		using std::swap;

		swap(*this, rhs);

		return *this;
	}

	inline ~TGraphicsObjectPool() override
	{
		clear();

		// All objects should be removed at the end
		PH_ASSERT_EQ(m_numObjs, 0);
	}

	inline Object* accessItem(const Handle& handle) override
	{
		return get(handle);
	}

	inline const Object* viewItem(const Handle& handle) const override
	{
		return get(handle);
	}

	/*! @brief Place `obj` at the storage slot indicated by `handle`.
	Complexity: Amortized O(1). O(1) if `hasFreeSpace()` returns true.
	@return The handle `obj` was created on. Same as the input `handle`.
	*/
	inline Handle createAt(const Handle& handle, Object obj)
	{
		if(!isFresh(handle))
		{
			throw_formatted<IllegalOperationException>(
				"creating object with stale handle ({})",
				handle.toString());
		}

		// Potentially create new storage space
		const Index objIdx = handle.getIndex();
		if(objIdx >= capacity())
		{
			if(capacity() == maxCapacity())
			{
				throw_formatted<OverflowException>(
					"Storage size will exceed the maximum amount Index type can hold (max={})",
					maxCapacity());
			}

			grow(nextCapacity(capacity()));
		}

		PH_ASSERT_LT(objIdx, m_generations.size());

		// `Object` was manually destroyed. No need for storing the returned pointer nor using
		// `std::launder()` on each use (same object type with exactly the same storage location), 
		// see C++ standard [basic.life] section 8 (https://timsong-cpp.github.io/cppwp/n4659/basic.life#8).
		std::construct_at(m_storageMemory.get() + objIdx, std::move(obj));

		PH_ASSERT_NE(m_generations[objIdx], handle.getGeneration());
		m_generations[objIdx] = handle.getGeneration();

		++m_numObjs;

		return handle;
	}

	/*! @brief Remove the object at the storage slot indicated by `handle`.
	Complexity: O(1).
	@return The handle for creating a new object on the storage slot that was indicated by `handle`.
	The returned handle must not be discarded as there are no defined means to retrieve the handle
	for a free storage slot.
	*/
	[[nodiscard]]
	inline Handle removeAt(const Handle& handle)
	{
		if(!isFresh(handle))
		{
			throw_formatted<IllegalOperationException>(
				"removing object with stale handle ({})",
				handle.toString());
		}

		const Index objIdx = handle.getIndex();
		PH_ASSERT_LT(objIdx, capacity());

		// This is not necessary as we are dealing with trivially destructible objects,
		// just to be safe and consistent
		std::destroy_at(m_storageMemory.get() + objIdx);

		const Generation newGeneration = Handle::nextGeneration(handle.getGeneration());
		m_generations[objIdx] = newGeneration;
		--m_numObjs;

		return Handle(handle.getIndex(), newGeneration);
	}

	inline void clear()
	{
		// We are not tracking slot validity, no chance & no need to destruct objects manually as
		// we are dealing with trivially destructible objects
		m_numObjs = 0;
	}

	/*!
	Complexity: O(1).
	*/
	inline Object* get(const Handle& handle)
	{
		return isFresh(handle) ? (m_storageMemory.get() + handle.getIndex()) : nullptr;
	}

	/*!
	Complexity: O(1).
	*/
	inline const Object* get(const Handle& handle) const
	{
		return isFresh(handle) ? (m_storageMemory.get() + handle.getIndex()) : nullptr;
	}

	inline Index numObjects() const
	{
		PH_ASSERT_LE(m_numObjs, capacity());
		return m_numObjs;
	}

	inline Index numFreeSpace() const
	{
		PH_ASSERT_LE(numObjects(), capacity());
		return capacity() - numObjects();
	}

	inline Index capacity() const
	{
		PH_ASSERT_LE(m_generations.size(), maxCapacity());
		return static_cast<Index>(m_generations.size());
	}

	/*!
	@return Wether this pool contains any object.
	*/
	inline bool isEmpty() const
	{
		return numObjects() == 0;
	}

	inline bool isFresh(const Handle& handle) const
	{
		return handle.getIndex() < m_generations.size() &&
		       handle.getGeneration() == m_generations[handle.getIndex()];
	}

	inline static constexpr Index maxCapacity()
	{
		return std::numeric_limits<Index>::max();
	}

	inline friend void swap(TGraphicsObjectPool& first, TGraphicsObjectPool& second)
	{
		// Enable ADL
		using std::swap;

		swap(first.m_storageMemory, second.m_storageMemory);
		swap(first.m_generations, second.m_generations);
		swap(first.m_numObjs, second.m_numObjs);
	}

private:
	inline void grow(const Index newCapacity)
	{
		const Index oldCapacity = capacity();
		PH_ASSERT_GT(newCapacity, oldCapacity);

		const auto requiredMemorySize = newCapacity * sizeof(Object);
		const auto alignmentSize = std::lcm(alignof(Object), os::get_L1_cache_line_size_in_bytes());
		const auto totalMemorySize = math::next_multiple(requiredMemorySize, alignmentSize);

		// Create new item storage and move item over

		TAlignedMemoryUniquePtr<Object> newStorageMemory = 
			make_aligned_memory<Object>(totalMemorySize, alignmentSize);
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
		m_generations.resize(newCapacity, Handle::nextGeneration(Handle::INVALID_GENERATION));

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
	TAlignedMemoryUniquePtr<Object> m_storageMemory;
	std::vector<Generation> m_generations;
	Index m_numObjs;
};

}// end namespace ph::editor
