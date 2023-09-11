#pragma once

#include "EditorCore/Storage/fwd.h"

#include <Utility/Concurrent/TAtomicQuasiQueue.h>
#include <Common/assertion.h>

#include <queue>
#include <utility>

namespace ph::editor
{

/*! @brief Sequential handle dispatcher meant for single-threaded use.
*/
template<CWeakHandle Handle>
class THandleDispatcher final
{
	using Index = typename Handle::IndexType;
	using Generation = typename Handle::GenerationType;

public:
	using HandleType = Handle;

	inline THandleDispatcher()
		: m_handles()
		, m_nextNewIdx(0)
	{}

	inline THandleDispatcher(const THandleDispatcher& other)
		: m_handles(other.m_handles)
		, m_nextNewIdx(other.m_nextNewIdx)
	{}

	inline THandleDispatcher(THandleDispatcher&& other) noexcept
		: THandleDispatcher()
	{
		swap(*this, other);
	}

	inline THandleDispatcher& operator = (THandleDispatcher rhs)
	{
		swap(*this, rhs);

		return *this;
	}

	/*! @brief Get one handle.
	*/
	[[nodiscard]]
	inline Handle dispatchOne()
	{
		if(!m_handles.empty())
		{
			Handle handle = m_handles.front();
			m_handles.pop();
			return handle;
		}

		// Create new handle if we cannot obtain an existing one
		constexpr auto initialGeneration = Handle::nextGeneration(Handle::INVALID_GENERATION);
		const Index newIdx = m_nextNewIdx++;
		return Handle(newIdx, initialGeneration);
	}

	/*! @brief Recycle one handle.
	*/
	inline void returnOne(const Handle& handle)
	{
		PH_ASSERT(handle);
		m_handles.push(handle);
	}

	inline friend void swap(THandleDispatcher& first, THandleDispatcher& second) noexcept
	{
		// Enable ADL
		using std::swap;

		swap(first.m_handles, second.m_handles);
		swap(first.m_nextNewIdx, second.m_nextNewIdx);
	}

private:
	std::queue<Handle> m_handles;
	Index m_nextNewIdx;
};

}// end namespace ph::editor
