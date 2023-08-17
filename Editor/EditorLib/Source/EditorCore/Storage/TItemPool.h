#pragma once

#include "EditorCore/Storage/TWeakHandle.h"

#include <Common/assertion.h>
#include <Utility/exception.h>

#include <cstddef>
#include <vector>
#include <utility>

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
		const Index freeIdx = popFreeSpace();
		m_storage[freeIdx].u_item = std::move(item);
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

		pushFreeSpace(handle.getIndex());
	}

	/*!
	Complexity: O(1).
	*/
	inline Item* get(const HandleType& handle)
	{
		return isFresh(handle) ? &(m_storage[handle.getIndex()].u_item) : nullptr;
	}

	/*!
	Complexity: O(1).
	*/
	inline const Item* get(const HandleType& handle) const
	{
		return isFresh(handle) ? &(m_storage[handle.getIndex()].u_item) : nullptr;
	}

	inline Index numItems() const
	{
		PH_ASSERT_LE(numFreeSpace(), m_storage.size());
		return m_storage.size() - numFreeSpace();
	}

	inline Index numFreeSpace() const
	{
		return m_numFreeIndices;
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
	inline Index popFreeSpace()
	{
		PH_ASSERT_EQ(m_storage.size(), m_storageStates.size());

		// Existing space available, get it and update free list head
		if(m_nextFreeIdx != HandleType::INVALID_INDEX)
		{
			PH_ASSERT_GT(m_numFreeIndices, 0);
			const Index freeIdx = m_nextFreeIdx;
			--m_numFreeIndices;

			PH_ASSERT_LT(freeIdx, m_storage.size());
			m_nextFreeIdx = m_storage[freeIdx].u_freedItem.nextFreeIdx;
			m_storageStates[freeIdx].isFreed = false;
			return freeIdx;
		}
		// Create new storage space
		else
		{
			PH_ASSERT_EQ(m_numFreeIndices, 0);
			PH_ASSERT_EQ(m_nextFreeIdx, HandleType::INVALID_INDEX);

			StoredItem storedItem;
			storedItem.u_freedItem = FreeListItem::makeInvalid();

			m_storage.push_back(storedItem);
			m_storageStates.push_back(StorageState{});
			return m_storage.size() - 1;
		}
	}

	inline void pushFreeSpace(const Index freeIdx)
	{
		PH_ASSERT_LT(freeIdx, m_storage.size());

		FreeListItem freedItem;
		freedItem.nextFreeIdx = m_nextFreeIdx;

		m_storage[freeIdx].u_freedItem = freedItem;
		++m_storageStates[freeIdx].generation;
		m_storageStates[freeIdx].isFreed = true;
		m_nextFreeIdx = freeIdx;
		++m_numFreeIndices;
	}

private:
	struct FreeListItem
	{
		Index nextFreeIdx;

		inline static FreeListItem makeInvalid()
		{
			FreeListItem freedItem;
			freedItem.nextFreeIdx = HandleType::INVALID_INDEX;
			return freedItem;
		}
	};

	union StoredItem
	{
		FreeListItem u_freedItem;
		Item u_item;
	};

	struct StorageState
	{
		Index generation = 0;
		bool isFreed = true;
	};

	std::vector<StoredItem> m_storage;
	std::vector<StorageState> m_storageStates;
	Index m_nextFreeIdx = HandleType::INVALID_INDEX;
	Index m_numFreeIndices = 0;
};

}// end namespace ph::editor
