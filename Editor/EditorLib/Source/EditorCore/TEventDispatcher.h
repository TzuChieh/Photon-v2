#pragma once

#include "EditorCore/Event/Event.h"

#include <Utility/TFunction.h>
#include <Common/primitive_type.h>
#include <Common/assertion.h>
#include <Utility/TUniquePtrVector.h>

#include <limits>
#include <type_traits>
#include <utility>

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
	bool removeListener(Listener* listener);

	template<typename DispatchFunc>
	void dispatch(const EventType& e, DispatchFunc dispatchFunc) const;

private:
	TUniquePtrVector<Listener> m_listeners;
};

template<typename EventType>
inline auto TEventDispatcher<EventType>::addListener(Listener listener)
	-> Listener*
{
	return m_listeners.add(std::make_unique<Listener>(std::move(listener)));
}

template<typename EventType>
inline bool TEventDispatcher<EventType>::removeListener(Listener* const listener)
{
	return m_listeners.remove(listener);
}

template<typename EventType>
template<typename DispatchFunc>
inline void TEventDispatcher<EventType>::dispatch(const EventType& e, DispatchFunc dispatchFunc) const
{
	static_assert(std::is_invocable_v<DispatchFunc, EventType, Listener>,
		"DispatchFunc must take (EventType, Listener).");

	for(const Listener& listener : m_listeners)
	{
		dispatchFunc(e, *listener);
	}
}

}// end namespace ph
