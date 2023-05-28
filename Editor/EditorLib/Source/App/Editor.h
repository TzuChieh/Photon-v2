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
#include "App/EditContext.h"
#include "App/Event/EditContextUpdateEvent.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Utility/TFunction.h>
#include <Utility/TUniquePtrVector.h>
#include <Utility/INoCopyAndMove.h>

#include <cstddef>
#include <list>
#include <string>

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

	/*! @brief Called once before the update loop begins.
	*/
	void start();

	void update(const MainThreadUpdateContext& ctx);
	void renderUpdate(const MainThreadRenderUpdateContext& ctx);
	void createRenderCommands(RenderThreadCaller& caller);
	void beforeUpdateStage();
	void afterUpdateStage();
	void beforeRenderStage();
	void afterRenderStage();

	/*! @brief Called once after the update loop ends.
	*/
	void stop();

	void renderCleanup(RenderThreadCaller& caller);
	void cleanup();

	std::size_t createScene(const std::string& name = "");
	DesignerScene* getScene(std::size_t sceneIndex) const;
	DesignerScene* getActiveScene() const;
	void setActiveScene(std::size_t sceneIndex);
	void removeScene(std::size_t sceneIndex);
	std::size_t numScenes() const;

	EditContext getEditContext() const;

private:
	struct PendingRemovalScene
	{
		std::unique_ptr<DesignerScene> scene;
		bool hasRenderCleanupDone = false;
		bool hasCleanupDone = false;
	};

	TUniquePtrVector<DesignerScene> m_scenes;
	std::list<PendingRemovalScene> m_removingScenes;
	DesignerScene* m_activeScene = nullptr;

	void loadDefaultScene();
	void renderCleanupRemovingScenes(RenderThreadCaller& caller);
	void cleanupRemovingScenes();

// Event System
public:
	TEventDispatcher<KeyDownEvent> onKeyDown;
	TEventDispatcher<KeyUpEvent> onKeyUp;
	TEventDispatcher<DisplayFramebufferResizeEvent> onDisplayFramebufferResize;
	TEventDispatcher<SceneFramebufferResizeEvent> onSceneFramebufferResize;
	TEventDispatcher<DisplayCloseEvent> onDisplayClose;
	TEventDispatcher<AppModuleActionEvent> onAppModuleAction;
	TEventDispatcher<EditContextUpdateEvent> onEditContextUpdate;

	template<typename EventType>
	void postEvent(const EventType& e, TEventDispatcher<EventType>& eventDispatcher);

private:
	template<typename EventType>
	static void dispatchEventToListeners(
		const EventType& e, 
		TEventDispatcher<EventType>& eventDispatcher);

	void flushAllEvents();

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
