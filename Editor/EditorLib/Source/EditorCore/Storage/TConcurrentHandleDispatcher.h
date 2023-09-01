#pragma once

#include "EditorCore/Storage/fwd.h"

#include <Utility/Concurrent/TAtomicQuasiQueue.h>
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
		swap(*this, other);
	}

	inline TConcurrentHandleDispatcher& operator = (TConcurrentHandleDispatcher&& rhs) noexcept
	{
		swap(*this, rhs);

		return *this;
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

	inline friend void swap(TConcurrentHandleDispatcher& first, TConcurrentHandleDispatcher& second) noexcept
	{
		// Enable ADL
		using std::swap;

		swap(first.m_handles, second.m_handles);

		Index firstIdx = first.m_nextNewIdx.exchange(second.m_nextNewIdx.load());
		second.m_nextNewIdx.store(firstIdx);
	}

private:
	TAtomicQuasiQueue<Handle> m_handles;
	std::atomic<Index> m_nextNewIdx;
};

}// end namespace ph::editor
