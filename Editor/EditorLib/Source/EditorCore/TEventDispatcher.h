#pragma once

#include "EditorCore/Event/Event.h"
#include "EditorCore/TEventListener.h"

#include <Utility/TFunction.h>
#include <Common/primitive_type.h>
#include <Common/assertion.h>
#include <Common/exceptions.h>
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
	using Listener = TEventListener<EventType>;

public:
	Listener* addListener(Listener listener);

	/*! @biref Remove a listener that was added to this dispatcher.
	The listener will not be removed immediately. Instead, @p listener will be marked for removal,
	and actually be removed during dispatch. The removal process is delayed so calling
	`removeListener()` in an event is valid and will not invalidate any listeners while they are
	still in use. If the delayed removal is not desired, use `removeListenerImmediately()` to
	immediately remove the listener.
	@param listener Listener to be removed. Must be one of the added listeners or be null (no-op in
	this case).
	*/
	void removeListener(Listener* listener);

	/*! @biref Immediately remove a listener that was added to this dispatcher.
	@param listener Listener to remove. Must be one of the added listeners or be null (no-op in
	this case).
	*/
	void removeListenerImmediately(Listener* listener);

	/*! @biref Submit an event to the listeners.
	@param e The event to submit.
	@param dispatchFunc A customizable function of the form `(EventType, Listener)` that governs how
	the event is submitted to listeners. Generally a call to listener, e.g., `listener(e)`, is expected
	to be run (either directly or indirectly) through `dispacthFunc`.
	*/
	template<typename DispatchFunc>
	void dispatch(const EventType& e, DispatchFunc dispatchFunc);

	void dispatch(const EventType& e);

private:
	TUniquePtrVector<Listener> m_listeners;
	std::vector<Listener*> m_listenersToRemove;
	bool m_isDispatching = false;
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
	if(!listener)
	{
		return;
	}

	// Add to the pending-to-remove queue, will actually be removed in `dispatch()`
	m_listenersToRemove.push_back(listener);
}

template<typename EventType>
inline void TEventDispatcher<EventType>::removeListenerImmediately(Listener* const listener)
{
	if(!listener)
	{
		return;
	}

	if(m_isDispatching)
	{
		throw IllegalOperationException(
			"Cannot remove listeners during dispatch. Use removeListener(1) instead.");
	}

	const auto removedListener = m_listeners.remove(listener);
	PH_ASSERT(removedListener != nullptr);
}

template<typename EventType>
template<typename DispatchFunc>
inline void TEventDispatcher<EventType>::dispatch(const EventType& e, DispatchFunc dispatchFunc)
{
	static_assert(std::is_invocable_v<DispatchFunc, EventType, Listener>,
		"DispatchFunc must take (EventType, Listener).");

	// Only actually remove listeners before dispatch--so other listeners can be kept valid
	// during dispatch. Also, removal is done before dispatch so calls to `removeListener()` 
	// are respected. Calling `removeListener()` during dispatch will take effect in the next dispatch.
	for(Listener* listener : m_listenersToRemove)
	{
		removeListenerImmediately(listener);
	}
	m_listenersToRemove.clear();

	// Iterate through all listeners and invoke the dispatch function on each of them. The code is
	// aware of that listeners might get added/removed (calling `addListener()` and `removeListener()`)
	// in the call to dispatch function. Listeners added/removed in the dispatch function will only
	// participate in the next call to `dispatch()`.

	// Record current count so newly added listeners will not get involved this time
	const auto numListeners = m_listeners.size();

	// Guard against nested dispatch call
	PH_ASSERT(!m_isDispatching);

	m_isDispatching = true;
	for(std::size_t listenerIdx = 0; listenerIdx < numListeners; ++listenerIdx)
	{
		dispatchFunc(e, *m_listeners[listenerIdx]);
	}
	m_isDispatching = false;
}

template<typename EventType>
inline void TEventDispatcher<EventType>::dispatch(const EventType& e)
{
	dispatch(
		e, 
		[](const EventType& e, const Listener& listener)
		{
			listener(e);
		});
}

}// end namespace ph
