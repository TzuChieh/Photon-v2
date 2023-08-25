#pragma once

#include "RenderCore/Storage/fwd.h"
#include "EditorCore/Storage/TWeakHandle.h"

#include <Utility/Concurrent/TAtomicQueue.h>
#include <Common/assertion.h>

#include <atomic>

namespace ph::editor
{

template<CWeakHandle Handle>
class TConcurrentHandleDispatcher final
{
	using Index = typename Handle::IndexType;
	using Generation = typename Handle::GenerationType;

public:
	using HandleType = Handle;

	inline TConcurrentHandleDispatcher()
		: m_handles()
		, m_nextNewIdx(0)
	{}

	/*! @brief Get one handle.
	@note Thread-safe.
	*/
	inline Handle dispatchOne()
	{
		Handle handle;
		if(m_handles.tryDequeue(&handle))
		{
			return handle;
		}

		// Create new handle if we cannot obtain an existing one
		const Index newIdx = m_nextNewIdx.fetch_add(1, std::memory_order_relaxed);
		const Generation newGeneration = Handle::nextGeneration(Handle::INVALID_GENERATION);
		return Handle(newIdx, newGeneration);
	}

	/*! @brief Recycle one handle.
	@note Thread-safe.
	*/
	inline void returnOne(const Handle& handle)
	{
		PH_ASSERT(handle);
		m_handles.enqueue(handle);
	}

private:
	TAtomicQueue<Handle> m_handles;
	std::atomic<Index> m_nextNewIdx;
};

}// end namespace ph::editor
