#pragma once

#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/assertion.h>
#include <Utility/exception.h>

#include <cstddef>
#include <vector>
#include <utility>
#include <memory>

namespace ph::editor
{

template<typename Item, typename Index = std::size_t, typename Generation = Index>
class TItemPool final
{
public:
	using HandleType = TWeakHandle<Item, Index, Generation>;

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
	inline void remove(const HandleType& handle)
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
	inline Item* get(const HandleType& handle)
	{
		return isFresh(handle) ? &(m_storage[handle.getIndex()]) : nullptr;
	}

	/*!
	Complexity: O(1).
	*/
	inline const Item* get(const HandleType& handle) const
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

	inline bool isFresh(const HandleType& handle) const
	{
		return handle.getIndex() < m_storageStates.size() &&
		       handle.getGeneration() == m_storageStates[handle.getIndex()].generation;
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
