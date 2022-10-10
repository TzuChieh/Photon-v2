#pragma once

#include <Utility/TFunction.h>
#include <Common/primitive_type.h>
#include <Common/assertion.h>
#include <Utility/TSortedMap.h>

#include <limits>
#include <type_traits>
#include <utility>

namespace ph::editor
{

using EventListenerHandle = uint16;

template<typename EventType>
class TEventDispatcher final
{
public:
	using Listener = TFunction<void(const EventType& e)>;
	using Handle   = EventListenerHandle;

	static_assert(std::is_integral_v<Handle>);

private:
	constexpr static Handle INVALID_HANDLE = std::numeric_limits<Handle>::max();

public:
	Handle addListener(Listener listener);
	Listener removeListener(Handle handle);

	template<typename DispatchFunc>
	void dispatch(DispatchFunc dispatchFunc, EventType e) const;

private:
	TSortedMap<Handle, Listener> m_handleToListener;
};

template<typename EventType>
inline auto TEventDispatcher<EventType>::addListener(Listener listener)
	-> Handle
{
	// Check free list first for empty listener slot
	if(!m_freeHandles.empty())
	{
		Handle handle = m_freeHandles.back();
		m_freeHandles.pop_back();
		m_listeners[m_freeHandles] = std::move(listener);
		return handle;
	}
	// Otherwise, make new space for it
	else
	{
		m_listeners.push_back(std::move(listener));
		
		// Make sure we are not exceeding max. allowed listeners
		PH_ASSERT_LT(m_listeners.size() - 1, INVALID_HANDLE);

		return static_cast<Handle>(m_listeners.size() - 1);
	}
}

template<typename EventType>
inline auto TEventDispatcher<EventType>::removeListener(Handle handle)
	-> Listener
{
	if(handle == INVALID_HANDLE)
	{
		return Listener();
	}

	PH_ASSERT_LT(handle, m_listeners.size());
	Listener listener = std::move(m_listeners[handle]);

	// Recycle the handle
	m_freeHandles.push_back(handle);

	return listener;
}

template<typename EventType>
template<typename DispatchFunc>
inline void TEventDispatcher<EventType>::dispatch(DispatchFunc dispatchFunc, EventType e) const
{
	static_assert(std::is_invocable_v<DispatchFunc, Listener, EventType>,
		"DispatchFunc must take (Listener, EventType).");

	for(const Listener& listener : m_listeners)
	{
		dispatchFunc(listener, e);
	}
}

}// end namespace ph
