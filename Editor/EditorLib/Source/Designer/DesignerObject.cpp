#include "DesignerObject.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>

#include <utility>

namespace ph::editor
{

DesignerObject::DesignerObject(DesignerScene* const scene)
	: m_scene(scene)
	, m_children()
	, m_name()
	, m_state()
{
	PH_ASSERT(m_scene != nullptr);
}

DesignerObject::~DesignerObject()
{
	// Initializations must happen either in pairs or not happen at all
	PH_ASSERT(
		(m_state.has(EObjectState::Initialized) && m_state.has(EObjectState::Uninitialized)) ||
		(m_state.hasNo(EObjectState::Initialized) && m_state.hasNo(EObjectState::Uninitialized)));
	PH_ASSERT(
		(m_state.has(EObjectState::RenderInitialized) && m_state.has(EObjectState::RenderUninitialized)) ||
		(m_state.hasNo(EObjectState::RenderInitialized) && m_state.hasNo(EObjectState::RenderUninitialized)));
}

void DesignerObject::init()
{
	PH_ASSERT(m_state.hasNo(EObjectState::Initialized));
}

void DesignerObject::uninit()
{
	PH_ASSERT(m_state.hasNo(EObjectState::Uninitialized));
}

void DesignerObject::renderInit(RenderThreadCaller& caller)
{
	PH_ASSERT(m_state.hasNo(EObjectState::RenderInitialized));
}

void DesignerObject::renderUninit(RenderThreadCaller& caller)
{
	PH_ASSERT(m_state.hasNo(EObjectState::RenderUninitialized));
}

void DesignerObject::update(const MainThreadUpdateContext& ctx)
{}

void DesignerObject::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{}

void DesignerObject::createRenderCommands(RenderThreadCaller& caller)
{}

void DesignerObject::removeChild(const std::size_t childIndex, const bool isRecursive)
{
	if(childIndex >= m_children.size())
	{
		return;
	}

	DesignerObject* objToRemove = m_children[childIndex];
	if(isRecursive)
	{
		// Recursively remove all children, starting from the last child
		while(objToRemove->hasChildren())
		{
			objToRemove->removeChild(objToRemove->numChildren() - 1, isRecursive);
		}
	}

	// Actually remove from other children and the scene
	m_children.erase(m_children.begin() + childIndex);
	getScene().removeObject(objToRemove);
}

void DesignerObject::setName(std::string name)
{
	// TODO: ensure unique name in scene
	m_name = std::move(name);
}

void DesignerObject::setTick(const bool shouldTick)
{
	getScene().markObjectTickState(this, shouldTick);
}

void DesignerObject::setRenderTick(const bool shouldTick)
{
	getScene().markObjectRenderTickState(this, shouldTick);
}

}// end namespace ph::editor
