#pragma once

#include "App/TEventDispatcher.h"
#include "App/Event/KeyDownEvent.h"
#include "App/Event/KeyUpEvent.h"
#include "App/EditorEventQueue.h"

namespace ph::editor
{

class Editor final
{
public:
	template<typename EventType>
	void dispatchToEventQueue(const EventType& e, const TEventDispatcher<EventType>& eventDispatcher);

public:
	TEventDispatcher<KeyDownEvent> onKeyDown;
	TEventDispatcher<KeyUpEvent> onKeyUp;

private:
	EditorEventQueue m_eventQueue;

	// TODO: may require obj/entity add/remove event queue to support concurrent event (to avoid invalidating Listener on addListener())
};

template<typename EventType>
inline void Editor::dispatchToEventQueue(const EventType& e, const TEventDispatcher<EventType>& eventDispatcher)
{
	using Listener = typename TEventDispatcher<EventType>::Listener;

	eventDispatcher.dispatch(e,
		[this](const EventType& e, const Listener& listener)
		{
			m_eventQueue.add(
				[&listener, e]()
				{
					listener(e);
				});
		});
}

}// end namespace ph::editor
