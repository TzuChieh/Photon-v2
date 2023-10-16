#pragma once

#include "EditorCore/TClassEventDispatcher.h"
#include "EditorCore/Event/KeyPressedEvent.h"
#include "EditorCore/Event/KeyReleasedEvent.h"
#include "EditorCore/Event/DisplayFramebufferResizedEvent.h"
#include "EditorCore/Event/SceneFramebufferResizedEvent.h"
#include "EditorCore/Event/DisplayClosedEvent.h"
#include "EditorCore/Event/DisplayFocusChangedEvent.h"
#include "EditorCore/Event/AppModuleActionEvent.h"
#include "App/Event/DesignerSceneAddedEvent.h"
#include "App/Event/DesignerSceneRemovalEvent.h"
#include "App/Event/ActiveDesignerSceneChangedEvent.h"
#include "App/EditorEventQueue.h"
#include "App/HelpMenu/HelpMenu.h"
#include "App/Misc/DimensionHints.h"
#include "App/Misc/EditorStats.h"
#include "EditorCore/FileSystemExplorer.h"
#include "App/EditContext.h"
#include "App/Event/EditContextUpdatedEvent.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>
#include <Utility/TFunction.h>
#include <Utility/TUniquePtrVector.h>
#include <Utility/INoCopyAndMove.h>

#include <cstddef>
#include <list>
#include <string>
#include <memory>

namespace ph { class Path; }

namespace ph::editor
{

class DesignerScene;
class MainThreadUpdateContext;
class MainThreadRenderUpdateContext;
class RenderThreadCaller;
class EditorSettings;

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

	/*! @brief Create a new scene. The scene will be made active automatically.
	This method is for a scene that is being created for the first time. If a scene file already exists,
	use `loadScene()` for it.
	*/
	std::size_t createScene(const Path& workingDirectory, const std::string& name = "");

	std::size_t createSceneFromDescription(
		const Path& workingDirectory,
		const Path& sceneDescription,
		const std::string& name = "",
		const std::string& descName = "");

	/*! @brief Load a scene. The scene will be made active automatically.
	@param sceneFile Path to the scene file. Must points to a valid scene file.
	*/
	std::size_t loadScene(const Path& sceneFile);

	/*! @brief Save the active scene.
	*/
	void saveScene();

	DesignerScene* getScene(std::size_t sceneIndex) const;
	DesignerScene* getActiveScene() const;
	void setActiveScene(std::size_t sceneIndex);
	void removeScene(std::size_t sceneIndex);
	std::size_t numScenes() const;
	//std::string getUniqueSceneName(const std::string& intendedName) const;

	EditContext getEditContext() const;

	static constexpr std::size_t nullSceneIndex();

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
	bool m_isActiveScenePaused = false;

	/*! @brief Make a new scene.
	A brand new scene without any properties set (all defaulted to what the scene defined).
	*/
	std::size_t newScene();

	void loadDefaultScene();
	void cleanupRemovingScenes();
	void renderCleanupRemovingScenes(RenderThreadCaller& caller);

// Event System
public:
	template<typename EventType>
	using TEditorEventDispatcher = TClassEventDispatcher<EventType, Editor>;

	/*! @brief Editor events.
	Subscribe to editor event by adding listener to the corresponding dispatcher. Note that when
	listening to events from a non-permanent resource, be sure to unsubscribe to the event before
	the resource is cleaned up so the event system will not act on a dangling resource. 
	Unsubscribing to an event can be done by removing listeners from the dispatcher, e.g., by calling 
	`TEditorEventDispatcher::removeListenerImmediately()` (calling the non-immediate overload is 
	also acceptable).

	Editor events can also be submitted in a delayed/buffered manner, and with thread safety guarantee.
	To submit event like this, use `postEvent()`. Do not reference anything that might not live across
	frames/threads when posting event.
	
	Use the `dispatch()` method of event dispatchers to submit event immediately, this is useful for
	transient events that references data with limited lifespan (keep in mind submitting events 
	this way is not thread-safe).
	*/
	///@{
	TEditorEventDispatcher<KeyPressedEvent> onKeyPressed;
	TEditorEventDispatcher<KeyReleasedEvent> onKeyReleased;
	TEditorEventDispatcher<DisplayFramebufferResizedEvent> onDisplayFramebufferResized;
	TEditorEventDispatcher<SceneFramebufferResizedEvent> onSceneFramebufferResized;
	TEditorEventDispatcher<DisplayClosedEvent> onDisplayClosed;
	TEditorEventDispatcher<DisplayFocusChangedEvent> onDisplayFocusChanged;
	TEditorEventDispatcher<AppModuleActionEvent> onAppModuleAction;
	TEditorEventDispatcher<EditContextUpdatedEvent> onEditContextUpdated;
	TEditorEventDispatcher<DesignerSceneAddedEvent> onDesignerSceneAdded;
	TEditorEventDispatcher<DesignerSceneRemovalEvent> onDesignerSceneRemoval;
	TEditorEventDispatcher<ActiveDesignerSceneChangedEvent> onActiveDesignerSceneChanged;
	///@}

	/*! @brief Specify an event that is going to be dispatched by the dispatcher.
	The event will not be dispatched immediately--instead, it will be delayed (potentially to the next
	multiple frames).
	@note Thread-safe.
	*/
	template<typename EventType>
	void postEvent(const EventType& e, TEditorEventDispatcher<EventType>& eventDispatcher);

private:
	template<typename EventType>
	static void dispatchPostedEventToListeners(
		const EventType& e, 
		TEditorEventDispatcher<EventType>& eventDispatcher);

	void flushAllEvents();

	EditorEventQueue m_eventPostQueue;

	// TODO: may require obj/entity add/remove event queue to support concurrent event 
	// (to avoid invalidating Listener on addListener())
// End Event System

public:
	void loadSettings(const Path& settingsFile);
	void saveSettings(const Path& settingsFile) const;
	EditorSettings& getSettings() const;

private:
	std::shared_ptr<EditorSettings> m_settings;
};

}// end namespace ph::editor

#include "App/Editor.ipp"
