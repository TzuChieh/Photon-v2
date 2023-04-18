#include "Designer/DesignerScene.h"
#include "App/Editor.h"
#include "Designer/DesignerObject.h"

#include <ranges>

namespace ph::editor
{

PH_DEFINE_LOG_GROUP(DesignerScene, Designer);

DesignerScene::DesignerScene(Editor* const fromEditor)
	: m_objs()
	, m_rootObjs()
	, m_tickingObjs()
	, m_renderTickingObjs()
	, m_objActionQueue()
	, m_numObjActionsToProcess(0)
	, m_editor(fromEditor)
	, m_mainCamera()
{
	PH_ASSERT(m_editor != nullptr);
}

DesignerScene::~DesignerScene()
{
	// Make sure everything is cleaned up
	PH_ASSERT_EQ(m_objs.size(), 0);
}

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

	// Process queued actions
	for(std::size_t actionIdx = 0; actionIdx < m_numObjActionsToProcess; ++actionIdx)
	{
		ObjectAction& queuedAction = m_objActionQueue[actionIdx];
		if(queuedAction.action == EObjectAction::Remove)
		{
			// We always want to remove the object from the cache arrays
			std::erase(m_tickingObjs, queuedAction.obj);
			std::erase(m_renderTickingObjs, queuedAction.obj);
			if(queuedAction.obj->getState().has(EObjectState::Root))
			{
				std::erase(m_rootObjs, queuedAction.obj);
			}

			if(queuedAction.obj->getState().has(EObjectState::RenderUninitialized) &&
			   queuedAction.obj->getState().hasNo(EObjectState::Uninitialized))
			{
				queuedAction.obj->uninit();
				queuedAction.obj->getState().turnOn({EObjectState::Uninitialized});
			}

			if(queuedAction.obj->getState().has(EObjectState::Uninitialized))
			{
				std::unique_ptr<DesignerObject> removedObj = m_objs.remove(queuedAction.obj);
				PH_ASSERT(removedObj != nullptr);

				queuedAction.done();
			}
		}
		else if(
			queuedAction.action == EObjectAction::EnableTick && 
			queuedAction.obj->getState().hasNo(EObjectState::Ticking))
		{
			m_tickingObjs.push_back(queuedAction.obj);
			queuedAction.obj->getState().turnOn({EObjectState::Ticking});
			queuedAction.done();
		}
		else if(
			queuedAction.action == EObjectAction::DisableTick &&
			queuedAction.obj->getState().has(EObjectState::Ticking))
		{
			std::erase(m_tickingObjs, queuedAction.obj);
			queuedAction.obj->getState().turnOff({EObjectState::Ticking});
			queuedAction.done();
		}
		else if(
			queuedAction.action == EObjectAction::EnableRenderTick &&
			queuedAction.obj->getState().hasNo(EObjectState::RenderTicking))
		{
			m_renderTickingObjs.push_back(queuedAction.obj);
			queuedAction.obj->getState().turnOn({EObjectState::RenderTicking});
			queuedAction.done();
		}
		else if(
			queuedAction.action == EObjectAction::DisableRenderTick &&
			queuedAction.obj->getState().has(EObjectState::RenderTicking))
		{
			std::erase(m_renderTickingObjs, queuedAction.obj);
			queuedAction.obj->getState().turnOff({EObjectState::RenderTicking});
			queuedAction.done();
		}
	}// end process queued actions

	// Tick objects
	for(DesignerObject* obj : m_tickingObjs)
	{
		const bool isFullyInitialized = 
			obj->getState().hasAll({EObjectState::Initialized, EObjectState::RenderInitialized}) &&
			obj->getState().hasNone({EObjectState::Uninitialized, EObjectState::RenderUninitialized});

		if(isFullyInitialized)
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
		const bool isFullyInitialized = 
			obj->getState().hasAll({EObjectState::Initialized, EObjectState::RenderInitialized}) &&
			obj->getState().hasNone({EObjectState::Uninitialized, EObjectState::RenderUninitialized});

		if(isFullyInitialized)
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
		if(queuedAction.action == EObjectAction::Create && 
		   queuedAction.obj->getState().has(EObjectState::Initialized) &&
		   queuedAction.obj->getState().hasNo(EObjectState::RenderInitialized))
		{
			queuedAction.obj->renderInit(caller);
			queuedAction.obj->getState().turnOn({EObjectState::RenderInitialized});
			queuedAction.done();
		}
		else if(
			queuedAction.action == EObjectAction::Remove &&
			queuedAction.obj->getState().has(EObjectState::RenderInitialized) &&
			queuedAction.obj->getState().hasNo(EObjectState::RenderUninitialized))
		{
			queuedAction.obj->renderUninit(caller);
			queuedAction.obj->getState().turnOn({EObjectState::RenderUninitialized});
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

void DesignerScene::removeObject(DesignerObject* const obj)
{
	queueObjectAction(obj, EObjectAction::Remove);
}

void DesignerScene::queueObjectAction(DesignerObject* const obj, const EObjectAction objAction)
{
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
	for(auto& obj : std::views::reverse(m_objs))
	{
		if(obj->getState().has(EObjectState::RenderInitialized) &&
		   obj->getState().hasNo(EObjectState::RenderUninitialized))
		{
			obj->renderUninit(caller);
			obj->getState().turnOn({EObjectState::RenderUninitialized});
		}
	}
}

void DesignerScene::cleanup()
{
	// Uninitialize all objects in the reverse order
	for(auto& obj : std::views::reverse(m_objs))
	{
		if(obj->getState().has(EObjectState::Initialized) &&
		   obj->getState().hasNo(EObjectState::Uninitialized))
		{
			// Potentially detect a call order failure case (render cleanup was not called)
			if(obj->getState().has(EObjectState::RenderInitialized) &&
			   obj->getState().hasNo(EObjectState::RenderUninitialized))
			{
				PH_LOG_ERROR(DesignerScene,
					"invalid object cleanup state detected: object {} needs render cleanup first",
					obj->getName());
			}
			else
			{
				obj->uninit();
				obj->getState().turnOn({EObjectState::Uninitialized});
			}
		}
	}

	// Destruct all objects in the reverse order
	while(!m_objs.isEmpty())
	{
		m_objs.removeLast();
	}
}

}// end namespace ph::editor
