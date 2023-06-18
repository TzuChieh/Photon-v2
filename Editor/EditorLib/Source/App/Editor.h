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

namespace ph { class Path; }

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

	/*! @brief Create a new scene. The scene will be made active automatically.
	This method is for a scene that is being created for the first time. If a scene file already exists,
	use `loadScene()` for it.
	*/
	std::size_t createScene(const Path& workingDirectory, const std::string& name = "");

	std::size_t createSceneFromDescription(
		const Path& descriptionFile,
		const Path& workingDirectory, 
		const std::string& name = "");

	/*! @brief Load a scene. The scene will be made active automatically.
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

}// end namespace ph::editor

#include "App/Editor.ipp"
