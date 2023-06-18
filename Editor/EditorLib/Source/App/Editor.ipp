#pragma once

#include "App/Editor.h"

namespace ph::editor
{

inline DesignerScene* Editor::getScene(const std::size_t sceneIndex) const
{
	return m_scenes.get(sceneIndex);
}

inline DesignerScene* Editor::getActiveScene() const
{
	return m_activeScene;
}

inline std::size_t Editor::numScenes() const
{
	return m_scenes.size();
}

inline constexpr std::size_t Editor::nullSceneIndex()
{
	return static_cast<std::size_t>(-1);
}

template<typename EventType>
inline void Editor::postEvent(const EventType& e, TEventDispatcher<EventType>& eventDispatcher)
{
	using EventPostWork = EditorEventQueue::EventUpdateWork;

	// Event should be captured by value since exceution of queued works are delayed, there is no
	// guarantee that the original event object still lives by the time we execute the work.
	//
	// Work for some event `e` that is going to be posted by `eventDispatcher`. Do not reference 
	// anything that might not live across frames when constructing post work, as `postEvent()` 
	// can get called anywhere in a frame and the execution of post works may be delayed to 
	// next multiple frames.
	m_eventPostQueue.add(
		[&eventDispatcher, e]()
		{
			dispatchEventToListeners(e, eventDispatcher);
		});
}

template<typename EventType>
inline void Editor::dispatchEventToListeners(
	const EventType& e, 
	TEventDispatcher<EventType>& eventDispatcher)
{
	using Listener = typename TEventDispatcher<EventType>::Listener;

	eventDispatcher.dispatch(
		e, 
		[](const EventType& e, const Listener& listener)
		{
			listener(e);
		});
}

}// end namespace ph::editor
