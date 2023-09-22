#include "Designer/DesignerScene.h"
#include "App/Editor.h"
#include "Designer/DesignerObject.h"
#include "EditorCore/Thread/Threads.h"
#include "Render/RenderThreadCaller.h"
#include "Render/Scene.h"
#include "Render/System.h"

#include <Common/assertion.h>
#include <Common/profiling.h>
#include <Utility/exception.h>
#include <SDL/Introspect/SdlClass.h>

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
	, m_renderWorkQueue()
	, m_numSceneActionsToProcess(0)

	, m_editor(fromEditor)
	, m_rendererScene(nullptr)
	, m_rendererBindings()
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
	std::erase_if(m_sceneActionQueue,
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
			PH_ASSERT(obj->state().has(EObjectState::Ticking));
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
			PH_ASSERT(obj->state().has(EObjectState::RenderTicking));
			obj->renderUpdate(ctx);
		}
	}
}

void DesignerScene::createRenderCommands(RenderThreadCaller& caller)
{
	// Create the renderer scene if not already present. Must be the first thing to do, so subsequent
	// operations can access the renderer scene.
	if(!m_rendererScene)
	{
		auto debugName = "DesignerScene " + m_name + " (initial name)";
		auto rendererScene = std::make_unique<render::Scene>(debugName);
		m_rendererScene = rendererScene.get();

		caller.add(
			[scene = std::move(rendererScene)](render::System& sys) mutable
			{
				sys.addScene(std::move(scene));
			});
	}

	// Render tick objects
	// (this should idealy be placed after `renderUpdate()` without any object state change in between,
	// as many implementations expect `renderUpdate()` and `createRenderCommands()` are in pairs)
	for(DesignerObject* obj : m_renderTickingObjs)
	{
		if(isFullyInitialized(*obj))
		{
			PH_ASSERT(obj->state().has(EObjectState::RenderTicking));
			obj->createRenderCommands(caller);
		}
	}

	// Submit additionally added render works
	for(RenderWork& renderWork : m_renderWorkQueue)
	{
		const bool isObjUsable = renderWork.obj && isInitialized(*renderWork.obj);
		if(isObjUsable)
		{
			caller.add(std::move(renderWork.work));
		}
		else
		{
			PH_LOG_WARNING(DesignerScene,
				"One render work canceled (from object {}, init: {}, fully init: {}), please "
				"make sure to not enqueue render work outside of usable object lifetime.",
				get_object_debug_info(renderWork.obj), 
				renderWork.obj ? isInitialized(*renderWork.obj) : false,
				renderWork.obj ? isFullyInitialized(*renderWork.obj) : false);
		}
	}
	m_renderWorkQueue.clear();

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

	auto& objState = obj->state();
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

	auto& objState = obj->state();
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

	auto& objState = obj->state();
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
	enqueueObjectTickAction(obj, shouldTick);
}

void DesignerScene::changeObjectRenderTick(DesignerObject* const obj, const bool shouldTick)
{
	enqueueObjectRenderTickAction(obj, shouldTick);
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

	obj->state().turnOn({EObjectState::Root});
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

	if(obj->state().has(EObjectState::HasInitialized))
	{
		throw_formatted<IllegalOperationException>(
			"object {} has already been initialized",
			get_object_debug_info(obj));
	}

	obj->init();
	obj->state().turnOn({EObjectState::HasInitialized});

	enqueueCreateObjectAction(obj);
}

void DesignerScene::setObjectToDefault(DesignerObject* const obj)
{
	if(!obj)
	{
		return;
	}

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

	enqueueRemoveObjectAction(obj);
}

void DesignerScene::enqueueCreateObjectAction(DesignerObject* const obj)
{
	// Note that we are not ensuring `obj` is from this scene here, as `obj` is
	// still in its initialization process (with incomplete data) and such
	// information cannot be reliably tested.

	SceneAction action;
	action.updateTask = 
		[this, obj](const MainThreadUpdateContext& ctx)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->state();

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

			const bool hasInitialized = objState.has(EObjectState::HasInitialized);
			if(hasInitialized)
			{
				onObjectAdded.dispatch(DesignerObjectAddedEvent(obj, this));
			}
			return hasInitialized;
		};
	action.renderTask = 
		[this, obj](RenderThreadCaller& caller)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->state();

			if(objState.has(EObjectState::HasInitialized) &&
			   objState.hasNo(EObjectState::HasRenderInitialized))
			{
				obj->renderInit(caller);
				objState.turnOn({EObjectState::HasRenderInitialized});
			}

			return objState.has(EObjectState::HasRenderInitialized);
		};

	enqueueSceneAction(std::move(action));
}

void DesignerScene::enqueueRemoveObjectAction(DesignerObject* const obj)
{
	ensureOwnedByThisScene(obj);

	SceneAction action;
	action.updateTask = 
		[this, obj](const MainThreadUpdateContext& ctx)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->state();

			// Remove the object from cache arrays
			if(objState.has(EObjectState::Ticking))
			{
				const auto numErased = std::erase(m_tickingObjs, obj);
				PH_ASSERT_EQ(numErased, 1);
				objState.turnOff({EObjectState::Ticking});
			}
			if(objState.has(EObjectState::Selected))
			{
				const auto numErased = std::erase(m_selectedObjs, obj);
				PH_ASSERT_EQ(numErased, 1);
				objState.turnOff({EObjectState::Selected});
			}
			if(objState.has(EObjectState::Root))
			{
				const auto numErased = std::erase(m_rootObjs, obj);
				PH_ASSERT_EQ(numErased, 1);
				objState.turnOff({EObjectState::Root});
			}

			if(objState.has(EObjectState::HasRenderUninitialized) &&
			   objState.hasNo(EObjectState::HasUninitialized))
			{
				onObjectRemoval.dispatch(DesignerObjectRemovalEvent(obj, this));

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
			auto& objState = obj->state();

			// Remove the object from cache arrays
			if(objState.has(EObjectState::RenderTicking))
			{
				const auto numErased = std::erase(m_renderTickingObjs, obj);
				PH_ASSERT_EQ(numErased, 1);
				objState.turnOff({EObjectState::RenderTicking});
			}
			
			if(objState.has(EObjectState::HasRenderInitialized) &&
			   objState.hasNo(EObjectState::HasRenderUninitialized))
			{
				obj->renderUninit(caller);
				objState.turnOn({EObjectState::HasRenderUninitialized});
			}

			return objState.has(EObjectState::HasRenderUninitialized);
		};

	enqueueSceneAction(std::move(action));
}

void DesignerScene::enqueueObjectTickAction(DesignerObject* const obj, const bool shouldTick)
{
	ensureOwnedByThisScene(obj);

	SceneAction action;
	action.updateTask = 
		[this, obj, shouldTick](const MainThreadUpdateContext& ctx)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->state();

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

	enqueueSceneAction(std::move(action));
}

void DesignerScene::enqueueObjectRenderTickAction(DesignerObject* const obj, const bool shouldTick)
{
	ensureOwnedByThisScene(obj);

	SceneAction action;
	action.updateTask =
		[this, obj, shouldTick](const MainThreadUpdateContext& ctx)
		-> bool
		{
			PH_ASSERT(obj);
			auto& objState = obj->state();

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

	enqueueSceneAction(std::move(action));
}

void DesignerScene::enqueueSceneAction(SceneAction action)
{
	// We still allow new actions to be queued when the scene is paused
	m_sceneActionQueue.push_back(std::move(action));
}

void DesignerScene::renderCleanup(RenderThreadCaller& caller)
{
	// Render uninitialize all objects in the reverse order
	for(auto& obj : std::views::reverse(m_objStorage))
	{
		auto& objState = obj->state();

		if(obj &&
		   objState.has(EObjectState::HasRenderInitialized) &&
		   objState.hasNo(EObjectState::HasRenderUninitialized))
		{
			obj->renderUninit(caller);
			objState.turnOn({EObjectState::HasRenderUninitialized});
		}
	}

	// Remove the renderer scene in the end, after all other operations are done with it.
	if(m_rendererScene)
	{
		caller.add(
			[scene = m_rendererScene](render::System& sys)
			{
				sys.removeScene(scene);
			});
		m_rendererScene = nullptr;
	}
}

void DesignerScene::enqueueObjectRenderWork(DesignerObject* const obj, RenderWorkType work)
{
	ensureOwnedByThisScene(obj);

	m_renderWorkQueue.push_back({std::move(work), obj});
}

void DesignerScene::cleanup()
{
	// Uninitialize all objects in the reverse order
	for(auto& obj : std::views::reverse(m_objStorage))
	{
		auto& objState = obj->state();

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

void DesignerScene::addRendererBinding(DesignerRendererBinding binding)
{
	if(!binding.ownerObj)
	{
		return;
	}

	m_rendererBindings.push_back(std::move(binding));
}

void DesignerScene::removeRendererBinding(DesignerObject* ownerObj)
{
	auto numRemoved = std::erase_if(m_rendererBindings, 
		[ownerObj](const DesignerRendererBinding& binding)
		{
			return binding.ownerObj == ownerObj;
		});
	PH_ASSERT_LE(numRemoved, 1);
}

std::string DesignerScene::getUniqueObjectName(const std::string& intendedName)
{
	PH_PROFILE_SCOPE();

	int suffixNumber = 1;
	while(true)
	{
		// Generating a name sequence like "name", "name (2)", "name (3)", etc.
		const std::string generatedName = 
			intendedName +
			(suffixNumber == 1 ? "" : " (" + std::to_string(suffixNumber) + ")");

		bool foundDuplicatedName = false;
		for(auto& objRes : m_objStorage)
		{
			if(generatedName == objRes->getName())
			{
				foundDuplicatedName = true;
				break;
			}
		}

		if(!foundDuplicatedName)
		{
			return generatedName;
		}

		++suffixNumber;
	}

	PH_ASSERT_UNREACHABLE_SECTION();
	return "";
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

std::vector<const SdlClass*> DesignerScene::getAllObjectClasses()
{
	std::vector<const SdlClass*> classes;
	for(const auto& [clazz, maker] : classToObjMaker)
	{
		classes.push_back(clazz);
	}
	return classes;
}

}// end namespace ph::editor
