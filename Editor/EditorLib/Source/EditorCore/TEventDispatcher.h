#pragma once

#include "EditorCore/Event/Event.h"

#include <Utility/TFunction.h>
#include <Common/primitive_type.h>
#include <Common/assertion.h>
#include <Utility/TUniquePtrVector.h>

#include <limits>
#include <type_traits>
#include <utility>
#include <cstddef>
#include <vector>

namespace ph::editor
{

template<typename EventType>
class TEventDispatcher final
{
	static_assert(std::is_base_of_v<Event, EventType>);

public:
	using Listener = TFunction<void(const EventType& e)>;

public:
	Listener* addListener(Listener listener);
	void removeListener(Listener* listener);

	template<typename DispatchFunc>
	void dispatch(const EventType& e, DispatchFunc dispatchFunc);

private:
	TUniquePtrVector<Listener> m_listeners;
	std::vector<Listener*> m_listenersToRemove;
};

template<typename EventType>
inline auto TEventDispatcher<EventType>::addListener(Listener listener)
	-> Listener*
{
	return m_listeners.add(std::make_unique<Listener>(std::move(listener)));
}

template<typename EventType>
inline void TEventDispatcher<EventType>::removeListener(Listener* const listener)
{
	// Add to the pending-to-remove queue, will actually be removed in `dispatch()`
	m_listenersToRemove.push_back(listener);
}

template<typename EventType>
template<typename DispatchFunc>
inline void TEventDispatcher<EventType>::dispatch(const EventType& e, DispatchFunc dispatchFunc)
{
	static_assert(std::is_invocable_v<DispatchFunc, EventType, Listener>,
		"DispatchFunc must take (EventType, Listener).");

	// Iterate through all listeners and invoke the dispatch function on each of them. The code is
	// aware of that listeners might get added/removed in the call to dispatch function. Listeners
	// added/removed in the dispatch function will only participate in the next call to `dispatch()`.

	// Record current count so newly added listeners will not get involved this time
	const auto numListeners = m_listeners.size();

	for(std::size_t listenerIdx = 0; listenerIdx < numListeners; ++listenerIdx)
	{
		dispatchFunc(e, *m_listeners[listenerIdx]);
	}

	// Only actually remove listeners after dispatch--so they can be kept valid during dispatch
	for(Listener* listener : m_listenersToRemove)
	{
		const auto removedListener = m_listeners.remove(listener);
		PH_ASSERT(removedListener != nullptr);
	}
	m_listenersToRemove.clear();
}

}// end namespace ph
