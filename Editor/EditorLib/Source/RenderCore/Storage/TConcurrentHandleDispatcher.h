#pragma once

#include "RenderCore/Storage/fwd.h"
#include "EditorCore/Storage/TWeakHandle.h"

#include <Utility/Concurrent/TAtomicQueue.h>
#include <Utility/IMoveOnly.h>
#include <Common/assertion.h>

#include <atomic>
#include <utility>

namespace ph::editor
{

template<CWeakHandle Handle>
class TConcurrentHandleDispatcher final : private IMoveOnly
{
	using Index = typename Handle::IndexType;
	using Generation = typename Handle::GenerationType;

public:
	using HandleType = Handle;

	inline TConcurrentHandleDispatcher()
		: m_handles()
		, m_nextNewIdx(0)
	{}

	inline TConcurrentHandleDispatcher(TConcurrentHandleDispatcher&& other) noexcept
		: TConcurrentHandleDispatcher()
	{
		*this = std::move(other);
	}

	inline TConcurrentHandleDispatcher& operator = (TConcurrentHandleDispatcher&& rhs) noexcept
	{
		m_handles = std::move(rhs.m_handles);
		m_nextNewIdx = rhs.m_nextNewIdx.load();
	}

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
		constexpr auto initialGeneration = Handle::nextGeneration(Handle::INVALID_GENERATION);
		const Index newIdx = m_nextNewIdx.fetch_add(1, std::memory_order_relaxed);
		return Handle(newIdx, initialGeneration);
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
