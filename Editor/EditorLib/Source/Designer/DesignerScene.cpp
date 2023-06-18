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

inline std::string get_object_debug_info(DesignerObject* const obj)
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
	: m_workingDirectory()

	, m_objStorage()
	, m_freeObjStorageIndices()
	, m_rootObjs()
	, m_tickingObjs()
	, m_renderTickingObjs()
	, m_objActionQueue()
	, m_numObjActionsToProcess(0)

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
		m_objActionQueue, 
		[](const ObjectAction& queuedAction)
		{
			return queuedAction.isDone();
		});

	// Record current number of actions so actions added later can be delayed to next cycle
	m_numObjActionsToProcess = m_objActionQueue.size();

	// No need to process further if the scene is currently paused
	if(isPaused())
	{
		return;
	}

	// Process queued actions
	// 
	// Cache arrays are modified here, as this is one of the few places that we can be sure that
	// those arrays are not in use (otherwise modifying them can invalidate their iterators)
	//
	for(std::size_t actionIdx = 0; actionIdx < m_numObjActionsToProcess; ++actionIdx)
	{
		ObjectAction& queuedAction = m_objActionQueue[actionIdx];
		DesignerObject* const obj = queuedAction.obj;
		auto& objState = obj->getState();

		if(queuedAction.action == EObjectAction::Create)
		{
			if(objState.hasNo(EObjectState::HasInitialized))
			{
				throw_formatted<IllegalOperationException>(
					"object {} is not initialized",
					get_object_debug_info(obj));
			}

			if(objState.has(EObjectState::Root))
			{
				m_rootObjs.push_back(obj);
			}
		}
		else if(queuedAction.action == EObjectAction::Remove)
		{
			// We always want to remove the object from cache arrays
			std::erase(m_tickingObjs, obj);
			std::erase(m_renderTickingObjs, obj);
			if(objState.has(EObjectState::Root))
			{
				const auto numErasedObjs = std::erase(m_rootObjs, obj);
				if(numErasedObjs != 1)
				{
					throw_formatted<IllegalOperationException>(
						"object {} is identified as root but does not appear (uniquely) in the root set "
						"({} were found)",
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

				queuedAction.done();
			}
		}
		else if(
			queuedAction.action == EObjectAction::EnableTick && 
			objState.hasNo(EObjectState::Ticking))
		{
			m_tickingObjs.push_back(obj);
			objState.turnOn({EObjectState::Ticking});
			queuedAction.done();
		}
		else if(
			queuedAction.action == EObjectAction::DisableTick &&
			objState.has(EObjectState::Ticking))
		{
			std::erase(m_tickingObjs, obj);
			objState.turnOff({EObjectState::Ticking});
			queuedAction.done();
		}
		else if(
			queuedAction.action == EObjectAction::EnableRenderTick &&
			objState.hasNo(EObjectState::RenderTicking))
		{
			m_renderTickingObjs.push_back(obj);
			objState.turnOn({EObjectState::RenderTicking});
			queuedAction.done();
		}
		else if(
			queuedAction.action == EObjectAction::DisableRenderTick &&
			objState.has(EObjectState::RenderTicking))
		{
			std::erase(m_renderTickingObjs, obj);
			objState.turnOff({EObjectState::RenderTicking});
			queuedAction.done();
		}
	}// end process queued actions

	// Tick objects
	for(DesignerObject* obj : m_tickingObjs)
	{
		if(isFullyInitialized(*obj))
		{
			PH_ASSERT(obj->getState().has(EObjectState::Ticking));
			obj->update(ctx);
		}
	}
}

void DesignerScene::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	// Tick objects
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
	// Process queued actions
	for(std::size_t actionIdx = 0; actionIdx < m_numObjActionsToProcess; ++actionIdx)
	{
		ObjectAction& queuedAction = m_objActionQueue[actionIdx];
		DesignerObject* const obj = queuedAction.obj;
		auto& objState = obj->getState();

		if(queuedAction.action == EObjectAction::Create && 
		   objState.has(EObjectState::HasInitialized) &&
		   objState.hasNo(EObjectState::HasRenderInitialized))
		{
			obj->renderInit(caller);
			objState.turnOn({EObjectState::HasRenderInitialized});
			queuedAction.done();
		}
		else if(
			queuedAction.action == EObjectAction::Remove &&
			objState.has(EObjectState::HasRenderInitialized) &&
			objState.hasNo(EObjectState::HasRenderUninitialized))
		{
			obj->renderUninit(caller);
			objState.turnOn({EObjectState::HasRenderUninitialized});
		}
	}// end process queued actions

	// Tick objects
	for(DesignerObject* obj : m_renderTickingObjs)
	{
		obj->createRenderCommands(caller);
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

void DesignerScene::markObjectTickState(DesignerObject* const obj, const bool markTick)
{
	PH_ASSERT(obj);

	if(markTick && obj->getState().hasNo(EObjectState::Ticking))
	{
		queueObjectAction(obj, EObjectAction::EnableTick);
	}
	else if(!markTick && obj->getState().has(EObjectState::Ticking))
	{
		queueObjectAction(obj, EObjectAction::DisableTick);
	}
}

void DesignerScene::markObjectRenderTickState(DesignerObject* const obj, const bool markTick)
{
	PH_ASSERT(obj);

	if(markTick && obj->getState().hasNo(EObjectState::RenderTicking))
	{
		queueObjectAction(obj, EObjectAction::EnableRenderTick);
	}
	else if(!markTick && obj->getState().has(EObjectState::RenderTicking))
	{
		queueObjectAction(obj, EObjectAction::DisableRenderTick);
	}
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

	queueObjectAction(obj, EObjectAction::Create);
}

void DesignerScene::setObjectToDefault(DesignerObject* const obj)
{
	if(!obj)
	{
		return;
	}

	const SdlClass* const clazz = obj->getDynamicSdlClass();
	PH_ASSERT(clazz);
	clazz->initDefaultResource(*obj);
}

void DesignerScene::deleteObject(DesignerObject* const obj)
{
	// It is a no-op if object is already empty
	if(!obj)
	{
		return;
	}

	queueObjectAction(obj, EObjectAction::Remove);
}

void DesignerScene::queueObjectAction(DesignerObject* const obj, const EObjectAction objAction)
{
	// We still allow new actions to be queued when the scene is paused

	if(obj)
	{
		m_objActionQueue.push_back({
			.obj = obj, 
			.action = objAction});
	}
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

}// end namespace ph::editor
