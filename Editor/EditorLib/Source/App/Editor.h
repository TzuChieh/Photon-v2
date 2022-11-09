#pragma once

#include "EditorCore/TEventDispatcher.h"
#include "EditorCore/Event/KeyDownEvent.h"
#include "EditorCore/Event/KeyUpEvent.h"
#include "EditorCore/Event/FrameBufferResizeEvent.h"
#include "EditorCore/Event/DisplayCloseEvent.h"
#include "App/EditorEventQueue.h"

namespace ph::editor
{

class Editor final
{
public:
	TEventDispatcher<KeyDownEvent> onKeyDown;
	TEventDispatcher<KeyUpEvent> onKeyUp;
	TEventDispatcher<FrameBufferResizeEvent> onFrameBufferResize;
	TEventDispatcher<DisplayCloseEvent> onDisplayClose;

	EditorEventQueue eventQueue;

public:
	template<typename EventType>
	void dispatchToEventQueue(const EventType& e, const TEventDispatcher<EventType>& eventDispatcher);


	// TODO: editor scene data

	// TODO: may require obj/entity add/remove event queue to support concurrent event 
	// (to avoid invalidating Listener on addListener())
};

template<typename EventType>
inline void Editor::dispatchToEventQueue(const EventType& e, const TEventDispatcher<EventType>& eventDispatcher)
{
	using Listener = typename TEventDispatcher<EventType>::Listener;

	eventDispatcher.dispatch(e,
		[this](const EventType& e, const Listener& listener)
		{
			eventQueue.add(
				[&listener, e]()
				{
					listener(e);
				});
		});
}

}// end namespace ph::editor
