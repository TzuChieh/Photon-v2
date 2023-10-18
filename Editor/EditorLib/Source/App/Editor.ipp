#pragma once

#include "App/Editor.h"

#include <type_traits>

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

inline std::size_t Editor::getSceneIndex(const DesignerScene* scene) const
{
	auto optIndex = m_scenes.indexOf(scene);
	return optIndex ? *optIndex : nullSceneIndex();
}

inline constexpr std::size_t Editor::nullSceneIndex()
{
	return static_cast<std::size_t>(-1);
}

template<typename EventType>
inline void Editor::postEvent(const EventType& e, TEditorEventDispatcher<EventType>& eventDispatcher)
{
	constexpr bool hasPostTraits = requires (EventType)
	{
		{ EventType::canPost } -> std::same_as<bool>;
	};

	if constexpr(hasPostTraits)
	{
		static_assert(EventType::canPost,
			"Attempting to post an event that does not allow event posting.");
	}

	// Posted event should be captured by value since exceution of queued works are delayed,
	// there is no guarantee that the original event object still lives by the time we
	// execute the work.
	//
	// Queued work is some event `e` that is going to be posted by `eventDispatcher`. Do not
	// reference anything that might not live across frames/threads when constructing post work,
	// as `postEvent()` can get called anywhere in a frame or from any thread and the execution
	// of post works may be delayed to next multiple frames.
	m_eventPostQueue.add(
		[&eventDispatcher, e]()
		{
			dispatchPostedEventToListeners(e, eventDispatcher);
		});
}

template<typename EventType>
inline void Editor::dispatchPostedEventToListeners(
	const EventType& e, 
	TEditorEventDispatcher<EventType>& eventDispatcher)
{
	using Listener = typename TEditorEventDispatcher<EventType>::Listener;

	eventDispatcher.dispatch(
		e, 
		[](const EventType& e, const Listener& listener)
		{
			listener(e);
		});
}

}// end namespace ph::editor
