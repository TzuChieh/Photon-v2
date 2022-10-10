#pragma once

#include "App/Event/Event.h"

#include <Utility/TFunction.h>
#include <Common/primitive_type.h>
#include <Common/assertion.h>
#include <Utility/TStableIndexDenseVector.h>

#include <limits>
#include <type_traits>
#include <utility>

namespace ph::editor
{

using EventListenerHandle = uint16;

inline constexpr auto INVALID_EVENT_LISTENER_HANDLE = std::numeric_limits<EventListenerHandle>::max();

template<typename EventType>
class TEventDispatcher final
{
	static_assert(std::is_base_of_v<Event, EventType>);

public:
	using Listener = TFunction<void(const EventType& e)>;
	using Handle   = EventListenerHandle;

	static_assert(std::is_integral_v<Handle>);

private:
	constexpr static Handle INVALID_HANDLE = std::numeric_limits<Handle>::max();

public:
	Handle addListener(Listener listener);
	bool removeListener(Handle handle);

	template<typename DispatchFunc>
	void dispatch(DispatchFunc dispatchFunc, EventType e) const;

private:
	TStableIndexDenseVector<Listener, Handle> m_listeners;

	// Just to make sure we are using consistent invalidity identifier
	static_assert(INVALID_EVENT_LISTENER_HANDLE == decltype(m_listeners)::INVALID_STABLE_INDEX);
};

template<typename EventType>
inline auto TEventDispatcher<EventType>::addListener(Listener listener)
	-> Handle
{
	return m_listeners.add(listener);
}

template<typename EventType>
inline bool TEventDispatcher<EventType>::removeListener(Handle handle)
{
	return m_listeners.remove(handle);
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
