#include "Designer/DesignerScene.h"
#include "App/Editor.h"
#include "Designer/DesignerObject.h"
#include "EditorCore/Thread/Threads.h"

#include <Utility/exception.h>
#include <SDL/Introspect/SdlClass.h>
#include <Common/assertion.h>

#include <utility>
#include <ranges>

namespace ph::editor
{

PH_DEFINE_LOG_GROUP(DesignerScene, Designer);

namespace
{

inline std::string get_object_debug_info(const DesignerObject* const obj)
{
	if(!obj)
	{
		return "(null)";
	}

	return obj->getName().empty() ? "(no name)" : obj->getName();
}

}// end anonymous namespace

std::unordered_map<const SdlClass*, DesignerScene::DynamicObjectMaker> DesignerScene::classToObjMaker;

DesignerScene::DesignerScene(Editor* const fromEditor)
	: onObjectAdded()
	, onObjectRemoval()
	
	, m_workingDirectory()

	, m_objStorage()
	, m_freeObjStorageIndices()
	, m_rootObjs()
	, m_tickingObjs()
	, m_renderTickingObjs()
	, m_selectedObjs()
	, m_sceneActionQueue()
	, m_numSceneActionsToProcess(0)

	, m_editor(fromEditor)
	, m_renderDescription()
	, m_mainCamera()
	, m_isPaused(false)

	, m_name()
{
	PH_ASSERT(m_editor != nullptr);
}

DesignerScene::DesignerScene(DesignerScene&& other) noexcept = default;

DesignerScene::~DesignerScene()
{
	// Make sure everything is cleaned up
	PH_ASSERT_EQ(m_objStorage.size(), 0);
}

DesignerScene& DesignerScene::operator = (DesignerScene&& rhs) noexcept = default;

void DesignerScene::update(const MainThreadUpdateContext& ctx)
{
	// Remove all done actions
	std::erase_if(
		m_sceneActionQueue,
		[](const SceneAction& action)
		{
			return action.isDone();
		});

	// Record current number of actions so actions added later can be delayed to next cycle
	m_numSceneActionsToProcess = m_sceneActionQueue.size();

	// No need to process further if the scene is currently paused
	if(isPaused())
	{
		return;
	}

	// Tick objects
	for(DesignerObject* obj : m_tickingObjs)
	{
		if(isFullyInitialized(*obj))
		{
			PH_ASSERT(obj->getState().has(EObjectState::Ticking));
			obj->update(ctx);
		}
	}

	// Process queued actions: Cache arrays can be modified in action tasks, as this is one of the 
	// few places that we can be sure that those arrays are not in use (otherwise modifying them
	// can invalidate their iterators)
	for(std::size_t actionIdx = 0; actionIdx < m_numSceneActionsToProcess; ++actionIdx)
	{
		SceneAction& action = m_sceneActionQueue[actionIdx];
		if(action.updateTask)
		{
			if(action.updateTask(ctx))
			{
				action.updateTask.unset();
			}
		}
	}
}

void DesignerScene::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	// Render tick objects
	for(DesignerObject* obj : m_renderTickingObjs)
	{
		if(isFullyInitialized(*obj))
		{
			PH_ASSERT(obj->getState().has(EObjectState::RenderTicking));
			obj->renderUpdate(ctx);
		}
	}
}

void DesignerScene::createRenderCommands(RenderThreadCaller& caller)
{
	// Render tick objects
	// (this should be placed after `renderUpdate()` without any object state change in between, as
	// many implementations expect `renderUpdate()` and `createRenderCommands()` are in pairs)
	for(DesignerObject* obj : m_renderTickingObjs)
	{
		if(isFullyInitialized(*obj))
		{
			PH_ASSERT(obj->getState().has(EObjectState::RenderTicking));
			obj->createRenderCommands(caller);
		}
	}

	// Process queued actions: Cache arrays can be modified in action tasks, as this is one of the 
	// few places that we can be sure that those arrays are not in use (otherwise modifying them
	// can invalidate their iterators).
	for(std::size_t actionIdx = 0; actionIdx < m_numSceneActionsToProcess; ++actionIdx)
	{
		SceneAction& action = m_sceneActionQueue[actionIdx];
		if(action.renderTask)
		{
			if(action.renderTask(caller))
			{
				action.renderTask.unset();
			}
		}
	}
}

void DesignerScene::beforeUpdateStage()
{}

void DesignerScene::afterUpdateStage()
{}

void DesignerScene::beforeRenderStage()
{}

void DesignerScene::afterRenderStage()
{}

bool DesignerScene::selectObject(DesignerObject* const obj)
{
	if(!obj)
	{
		return false;
	}

	ensureOwnedByThisScene(obj);

	auto& objState = obj->getState();
	if(objState.has(EObjectState::Selected))
	{
		return false;
	}

	objState.turnOn({EObjectState::Selected});
	m_selectedObjs.push_back(obj);
	return true;
}

bool DesignerScene::deselectObject(DesignerObject* const obj)
{
	if(!obj)
	{
		return false;
	}

	ensureOwnedByThisScene(obj);

	auto& objState = obj->getState();
	if(objState.hasNo(EObjectState::Selected))
	{
		return false;
	}

	objState.turnOff({EObjectState::Selected});
	const auto numErasedObjs = std::erase(m_selectedObjs, obj);
	if(numErasedObjs != 1)
	{
		PH_LOG_WARNING(DesignerScene,
			"Deselecting object {}, {} were found and deselected",
			get_object_debug_info(obj), numErasedObjs);
	}
	return true;
}

void DesignerScene::clearSelection()
{
	// Deselect every object in LIFO order
	while(!m_selectedObjs.empty())
	{
		deselectObject(m_selectedObjs.back());
	}
}

void DesignerScene::changeObjectVisibility(DesignerObject* const obj, const bool shouldBeVisible)
{
	ensureOwnedByThisScene(obj);

	auto& objState = obj->getState();
	if(shouldBeVisible && objState.has(EObjectState::Hidden))
	{
		// TODO

		objState.turnOff({EObjectState::Hidden});
	}
	else if(!shouldBeVisible && objState.hasNo(EObjectState::Hidden))
	{
		// TODO

		objState.turnOn({EObjectState::Hidden});
	}
}

void DesignerScene::changeObjectTick(DesignerObject* const obj, const bool shouldTick)
{
	queueObjectTickAction(obj, shouldTick);
}

void DesignerScene::changeObjectRenderTick(DesignerObject* const obj, const bool shouldTick)
{
	queueObjectRenderTickAction(obj, shouldTick);
}

DesignerObject* DesignerScene::newObject(
	const SdlClass* const clazz,
	const bool shouldInit,
	const bool shouldSetToDefault)
{
	PH_ASSERT(Threads::isOnMainThread());

	auto objMakerIter = classToObjMaker.find(clazz);
	if(objMakerIter == classToObjMaker.end())
	{
		throw_formatted<IllegalOperationException>(
			"target object ({}) is not a registered type",
			sdl::gen_pretty_name(clazz));
	}

	DesignerObject* obj = (objMakerIter->second)(*this);
	if(!obj)
	{
		return nullptr;
	}

	// Set object to default before init so default states can be considered
	if(shouldSetToDefault)
	{
		setObjectToDefault(obj);
	}

	if(shouldInit)
	{
		initObject(obj);
	}

	return obj;
}

DesignerObject* DesignerScene::newRootObject(
	const SdlClass* const clazz,
	const bool shouldInit,
	const bool shouldSetToDefault)
{
	DesignerObject* obj = newObject(clazz, shouldInit, shouldSetToDefault);
	if(!obj)
	{
		return nullptr;
	}

	obj->getState().turnOn({EObjectState::Root});
	obj->setParentScene(this);

	return obj;
}

std::shared_ptr<DesignerObject> DesignerScene::newSharedRootObject(
	const SdlClass* const clazz,
	const bool shouldInit,
	const bool shouldSetToDefault)
{
	DesignerObject* rootObj = newRootObject(clazz, shouldInit, shouldSetToDefault);
	if(!rootObj)
	{
		return nullptr;
	}

	return std::shared_ptr<DesignerObject>(
		rootObj,
		detail::TSharedObjectDeleter<DesignerObject>());
}

void DesignerScene::initObject(DesignerObject* const obj)
{
	if(!obj)
	{
		return;
	}

	if(obj->getState().has(EObjectState::HasInitialized))
	{
		throw_formatted<IllegalOperationException>(
			"object {} has already been initialized",
			get_object_debug_info(obj));
	}

	obj->init();
	obj->getState().turnOn({EObjectState::HasInitialized});

	queueCreateObjectAction(obj);
}

void DesignerScene::setObjectToDefault(DesignerObject* const obj)
{
	if(!obj)
	{
		return;
	}

	ensureOwnedByThisScene(obj);

	const SdlClass* const clazz = obj->getDynamicSdlClass();
	clazz->initDefaultResource(*obj);
}

void DesignerScene::deleteObject(
	DesignerObject* const obj,
	const bool shouldDeleteRecursively)
{
	// It is a no-op if object is already empty
	if(!obj)
	{
		return;
	}

	if(shouldDeleteRecursively)
	{
		obj->deleteAllChildren();
	}

	queueRemoveObjectAction(obj);
}

void DesignerScene::queueCreateObjectAction(DesignerObject* const obj)
{
	ensureOwnedByThisScene(obj);

	SceneAction action;
	action.updateTask = 
		[this, obj](const MainThreadUpdateContext& ctx)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->getState();

			if(objState.hasNo(EObjectState::HasInitialized))
			{
				// Object initialization is expected to be done before creation task
				throw_formatted<IllegalOperationException>(
					"object {} is not initialized",
					get_object_debug_info(obj));
			}

			if(objState.has(EObjectState::Root))
			{
				m_rootObjs.push_back(obj);
			}

			return objState.has(EObjectState::HasInitialized);
		};
	action.renderTask = 
		[this, obj](RenderThreadCaller& caller)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->getState();

			if(objState.has(EObjectState::HasInitialized) &&
			   objState.hasNo(EObjectState::HasRenderInitialized))
			{
				obj->renderInit(caller);
				objState.turnOn({EObjectState::HasRenderInitialized});
			}

			return objState.has(EObjectState::HasRenderInitialized);
		};

	queueSceneAction(std::move(action));
}

void DesignerScene::queueRemoveObjectAction(DesignerObject* const obj)
{
	ensureOwnedByThisScene(obj);

	SceneAction action;
	action.updateTask = 
		[this, obj](const MainThreadUpdateContext& ctx)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->getState();

			// We always want to remove the object from cache arrays
			std::erase(m_tickingObjs, obj);
			std::erase(m_renderTickingObjs, obj);
			std::erase(m_selectedObjs, obj);
			if(objState.has(EObjectState::Root))
			{
				const auto numErasedObjs = std::erase(m_rootObjs, obj);
				if(numErasedObjs != 1)
				{
					throw_formatted<IllegalOperationException>(
						"object {} is identified as root but does not appear (uniquely) in the "
						"root set ({} were found)",
						get_object_debug_info(obj), numErasedObjs);
				}
			}

			if(objState.has(EObjectState::HasRenderUninitialized) &&
			   objState.hasNo(EObjectState::HasUninitialized))
			{
				obj->uninit();
				objState.turnOn({EObjectState::HasUninitialized});
			}

			if(objState.has(EObjectState::HasUninitialized))
			{
				if(!removeObjectFromStorage(obj))
				{
					throw_formatted<IllegalOperationException>(
						"cannot remove object {} from storage",
						get_object_debug_info(obj));
				}
			}

			return objState.has(EObjectState::HasUninitialized);
		};
	action.renderTask = 
		[this, obj](RenderThreadCaller& caller)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->getState();
			
			if(objState.has(EObjectState::HasRenderInitialized) &&
			   objState.hasNo(EObjectState::HasRenderUninitialized))
			{
				obj->renderUninit(caller);
				objState.turnOn({EObjectState::HasRenderUninitialized});
			}

			return objState.has(EObjectState::HasRenderUninitialized);
		};

	queueSceneAction(std::move(action));
}

void DesignerScene::queueObjectTickAction(DesignerObject* const obj, const bool shouldTick)
{
	ensureOwnedByThisScene(obj);

	SceneAction action;
	action.updateTask = 
		[this, obj, shouldTick](const MainThreadUpdateContext& ctx)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->getState();

			if(shouldTick && objState.hasNo(EObjectState::Ticking))
			{
				m_tickingObjs.push_back(obj);
				objState.turnOn({EObjectState::Ticking});
			}
			else if(!shouldTick && objState.has(EObjectState::Ticking))
			{
				std::erase(m_tickingObjs, obj);
				objState.turnOff({EObjectState::Ticking});
			}

			return true;
		};

	queueSceneAction(std::move(action));
}

void DesignerScene::queueObjectRenderTickAction(DesignerObject* const obj, const bool shouldTick)
{
	ensureOwnedByThisScene(obj);

	SceneAction action;
	action.updateTask =
		[this, obj, shouldTick](const MainThreadUpdateContext& ctx)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->getState();

			if(shouldTick && objState.hasNo(EObjectState::RenderTicking))
			{
				m_renderTickingObjs.push_back(obj);
				objState.turnOn({EObjectState::RenderTicking});
			}
			else if(!shouldTick && objState.has(EObjectState::RenderTicking))
			{
				std::erase(m_renderTickingObjs, obj);
				objState.turnOff({EObjectState::RenderTicking});
			}

			return true;
		};

	queueSceneAction(std::move(action));
}

void DesignerScene::queueSceneAction(SceneAction action)
{
	// We still allow new actions to be queued when the scene is paused
	m_sceneActionQueue.push_back(std::move(action));
}

void DesignerScene::renderCleanup(RenderThreadCaller& caller)
{
	// Render uninitialize all objects in the reverse order
	for(auto& obj : std::views::reverse(m_objStorage))
	{
		auto& objState = obj->getState();

		if(obj &&
		   objState.has(EObjectState::HasRenderInitialized) &&
		   objState.hasNo(EObjectState::HasRenderUninitialized))
		{
			obj->renderUninit(caller);
			objState.turnOn({EObjectState::HasRenderUninitialized});
		}
	}
}

void DesignerScene::cleanup()
{
	// Uninitialize all objects in the reverse order
	for(auto& obj : std::views::reverse(m_objStorage))
	{
		auto& objState = obj->getState();

		if(obj &&
		   objState.has(EObjectState::HasInitialized) &&
		   objState.hasNo(EObjectState::HasUninitialized))
		{
			// Potentially detect a call order failure case (render cleanup was not called)
			if(objState.has(EObjectState::HasRenderInitialized) &&
			   objState.hasNo(EObjectState::HasRenderUninitialized))
			{
				PH_LOG_ERROR(DesignerScene,
					"invalid object cleanup state detected: object {} needs render cleanup first",
					get_object_debug_info(obj.get()));
			}
			else
			{
				obj->uninit();
				objState.turnOn({EObjectState::HasUninitialized});
			}
		}
	}

	// Destruct all objects in the reverse order
	while(!m_objStorage.isEmpty())
	{
		m_objStorage.removeLast();
	}

	m_freeObjStorageIndices.clear();
}

void DesignerScene::pause()
{
	m_isPaused = true;
}

void DesignerScene::resume()
{
	m_isPaused = false;
}

void DesignerScene::setWorkingDirectory(Path directory)
{
	m_workingDirectory = std::move(directory);
}

void DesignerScene::setName(std::string name)
{
	m_name = std::move(name);
}

void DesignerScene::ensureOwnedByThisScene(const DesignerObject* const obj) const
{
	if(!obj || &(obj->getScene()) != this)
	{
		throw_formatted<IllegalOperationException>(
			"Designer object {} is not from this scene ({})",
			get_object_debug_info(obj), getName());
	}
}

}// end namespace ph::editor
