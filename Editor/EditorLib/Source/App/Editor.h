#pragma once

#include "EditorCore/TEventDispatcher.h"
#include "EditorCore/Event/KeyDownEvent.h"
#include "EditorCore/Event/KeyUpEvent.h"
#include "EditorCore/Event/DisplayFramebufferResizeEvent.h"
#include "EditorCore/Event/SceneFramebufferResizeEvent.h"
#include "EditorCore/Event/DisplayCloseEvent.h"
#include "EditorCore/Event/AppModuleActionEvent.h"
#include "App/EditorEventQueue.h"
#include "App/HelpMenu/HelpMenu.h"
#include "App/Misc/DimensionHints.h"
#include "App/Misc/EditorStats.h"
#include "EditorCore/FileSystemExplorer.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Utility/TFunction.h>
#include <Utility/TUniquePtrVector.h>
#include <Utility/INoCopyAndMove.h>

#include <cstddef>
#include <vector>

namespace ph::editor
{

class DesignerScene;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;

class Editor final : private INoCopyAndMove
{
public:
	HelpMenu helpMenu;
	DimensionHints dimensionHints;
	EditorStats editorStats;
	FileSystemExplorer fileExplorer;

	Editor();
	~Editor();

	void updateScenes(const MainThreadUpdateContext& ctx);
	void renderUpdateScenes(const MainThreadRenderUpdateContext& ctx);
	void createRenderCommandsForScenes(RenderThreadCaller& caller);
	std::size_t createScene();
	DesignerScene* getScene(std::size_t sceneIndex) const;
	void removeScene(std::size_t sceneIndex);
	std::size_t numScenes() const;

private:
	TUniquePtrVector<DesignerScene> m_scenes;

// Event System
public:
	TEventDispatcher<KeyDownEvent> onKeyDown;
	TEventDispatcher<KeyUpEvent> onKeyUp;
	TEventDispatcher<DisplayFramebufferResizeEvent> onDisplayFramebufferResize;
	TEventDispatcher<SceneFramebufferResizeEvent> onSceneFramebufferResize;
	TEventDispatcher<DisplayCloseEvent> onDisplayClose;
	TEventDispatcher<AppModuleActionEvent> onAppModuleAction;

	void flushAllEvents();

	template<typename EventType>
	void postEvent(const EventType& e, TEventDispatcher<EventType>& eventDispatcher);

private:
	template<typename EventType>
	static void dispatchEventToListeners(
		const EventType& e, 
		TEventDispatcher<EventType>& eventDispatcher);

	EditorEventQueue m_eventPostQueue;

	// TODO: may require obj/entity add/remove event queue to support concurrent event 
	// (to avoid invalidating Listener on addListener())
// End Event System
};

inline DesignerScene* Editor::getScene(const std::size_t sceneIndex) const
{
	return m_scenes.get(sceneIndex);
}

inline std::size_t Editor::numScenes() const
{
	return m_scenes.size();
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
