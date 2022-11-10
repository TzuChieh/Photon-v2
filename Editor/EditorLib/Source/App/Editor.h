#pragma once

#include "EditorCore/TEventDispatcher.h"
#include "EditorCore/Event/KeyDownEvent.h"
#include "EditorCore/Event/KeyUpEvent.h"
#include "EditorCore/Event/FrameBufferResizeEvent.h"
#include "EditorCore/Event/DisplayCloseEvent.h"
#include "App/EditorEventQueue.h"

#include <Utility/TFunction.h>

#include <vector>

namespace ph::editor
{

class Editor final
{
public:
	TEventDispatcher<KeyDownEvent> onKeyDown;
	TEventDispatcher<KeyUpEvent> onKeyUp;
	TEventDispatcher<FrameBufferResizeEvent> onFrameBufferResize;
	TEventDispatcher<DisplayCloseEvent> onDisplayClose;

	void flushAllEvents();

public:
	template<typename EventType>
	void postEvent(const EventType& e, const TEventDispatcher<EventType>& eventDispatcher);

private:
	template<typename EventType>
	static void postEventToEventProcessQueue(
		const EventType&                    e, 
		const TEventDispatcher<EventType>&  eventDispatcher,
		std::vector<TFunction<void(void)>>& out_eventProcessQueue);

	EditorEventQueue                   m_eventPostQueue;
	std::vector<TFunction<void(void)>> m_eventProcessQueue;


	// TODO: editor scene data

	// TODO: may require obj/entity add/remove event queue to support concurrent event 
	// (to avoid invalidating Listener on addListener())
};

template<typename EventType>
inline void Editor::postEvent(const EventType& e, const TEventDispatcher<EventType>& eventDispatcher)
{
	using EventPostWork = EditorEventQueue::EventUpdateWork;

	// Event should be captured by value since exceution of queued works are delayed, there is no
	// guarantee that the original event object still lives by the time we execute the work.

	// Work for some event `e` that is going to be posted by `eventDispatcher`. Do not reference 
	// anything that might not live across frames when constructing post work, as `postEvent()` 
	// can get called anywhere in a frame and the execution of post works may be delayed to 
	// next multiple frames.
	m_eventPostQueue.add(
		[this, &eventDispatcher, e]()
		{
			postEventToEventProcessQueue(e, eventDispatcher, m_eventProcessQueue);
		});
}

template<typename EventType>
inline void Editor::postEventToEventProcessQueue(
	const EventType&                    e, 
	const TEventDispatcher<EventType>&  eventDispatcher,
	std::vector<TFunction<void(void)>>& out_eventProcessQueue)
{
	using Listener = typename TEventDispatcher<EventType>::Listener;

	// Work for queueing the event for later execution. Using a separate queue so it is
	// possible to do further preprocessing on all events later.
	auto queueEventForProcess = 
		[&out_eventProcessQueue](const EventType& e, const Listener& listener)
		{
			out_eventProcessQueue.push_back(
				[&listener, e]()
				{
					listener(e);
				});
		};

	eventDispatcher.dispatch(e, queueEventForProcess);
}

}// end namespace ph::editor
