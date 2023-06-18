#include "DesignerObject.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>
#include <Utility/utility.h>
#include <Utility/exception.h>

#include <utility>

namespace ph::editor
{

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

DesignerObject::DesignerObject()
	: m_parent()
	, m_name()
	, m_state()
	, m_sceneStorageIndex(static_cast<uint64>(-1))
{
	m_parent.u_object = nullptr;
}

DesignerObject::DesignerObject(const DesignerObject& other) = default;

DesignerObject::DesignerObject(DesignerObject&& other) noexcept = default;

DesignerObject::~DesignerObject()
{
	// Initializations must happen either in pairs or not happen at all
	PH_ASSERT(
		(m_state.has(EObjectState::HasInitialized) && m_state.has(EObjectState::HasUninitialized)) ||
		(m_state.hasNo(EObjectState::HasInitialized) && m_state.hasNo(EObjectState::HasUninitialized)));
	PH_ASSERT(
		(m_state.has(EObjectState::HasRenderInitialized) && m_state.has(EObjectState::HasRenderUninitialized)) ||
		(m_state.hasNo(EObjectState::HasRenderInitialized) && m_state.hasNo(EObjectState::HasRenderUninitialized)));
}

DesignerObject& DesignerObject::operator = (const DesignerObject& rhs) = default;

DesignerObject& DesignerObject::operator = (DesignerObject&& rhs) noexcept = default;

void DesignerObject::init()
{
	PH_ASSERT(m_state.hasNo(EObjectState::HasInitialized));
}

void DesignerObject::uninit()
{
	PH_ASSERT(m_state.hasNo(EObjectState::HasUninitialized));
}

void DesignerObject::renderInit(RenderThreadCaller& caller)
{
	PH_ASSERT(m_state.hasNo(EObjectState::HasRenderInitialized));
}

void DesignerObject::renderUninit(RenderThreadCaller& caller)
{
	PH_ASSERT(m_state.hasNo(EObjectState::HasRenderUninitialized));
}

void DesignerObject::update(const MainThreadUpdateContext& ctx)
{}

void DesignerObject::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{}

void DesignerObject::createRenderCommands(RenderThreadCaller& caller)
{}

DesignerObject* DesignerObject::newChild(
	const SdlClass* const clazz,
	const bool shouldInit,
	const bool shouldSetToDefault)
{
	if(!canHaveChildren())
	{
		return nullptr;
	}

	DesignerObject* childObj = getScene().newObject(clazz, shouldInit, shouldSetToDefault);
	if(!childObj)
	{
		return nullptr;
	}

	return addNewChild(childObj);
}

DesignerObject* DesignerObject::addNewChild(DesignerObject* const childObj)
{
	if(!childObj)
	{
		return nullptr;
	}

	if(childObj->getState().has(EObjectState::Root))
	{
		throw_formatted<IllegalOperationException>(
			"cannot add root object {} as a child of object {}",
			get_object_debug_info(childObj), getName());
	}

	// Ensure the child object is not already somebody's child
	if(childObj->getState().hasNo(EObjectState::Root) && 
	   m_parent.u_object != nullptr)
	{
		throw_formatted<IllegalOperationException>(
			"cannot add object {} as a child of object {} (parent already exists: {})",
			get_object_debug_info(childObj), getName(), m_parent.u_object->getName());
	}

	childObj->setParentObject(this);
	return addChild(childObj);
}

void DesignerObject::deleteChild(DesignerObject* const childObj)
{
	if(!canHaveChildren())
	{
		return;
	}

	if(childObj)
	{
		// Recursively remove all children, starting from the last child
		while(childObj->haveChildren())
		{
			childObj->deleteChild(childObj->getChildren().back());
		}
	}

	// Actually remove from parent and the scene
	if(removeChild(childObj))
	{
		getScene().deleteObject(childObj);
	}
	else
	{
		throw_formatted<IllegalOperationException>(
			"failed to remove child object {} from parent object {}",
			childObj ? childObj->getName() : "(null)", getName());
	}
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

DesignerScene& DesignerObject::getScene()
{
	return mutable_cast(std::as_const(*this).getScene());
}

const DesignerScene& DesignerObject::getScene() const
{
	if(getState().has(EObjectState::Root))
	{
		PH_ASSERT_MSG(m_parent.u_scene,
			"object was not properly initialized; did you call setParentScene()?");

		return *m_parent.u_scene;
	}
	else
	{
		// Traverse through the hierarchy finding the root scene
		PH_ASSERT(getParent());
		return getParent()->getScene();
	}
}

DesignerObject* DesignerObject::getParent()
{
	return mutable_cast(std::as_const(*this).getParent());
}

const DesignerObject* DesignerObject::getParent() const
{
	if(getState().hasNo(EObjectState::Root))
	{
		PH_ASSERT_MSG(m_parent.u_object,
			"object was not properly initialized; did you call setParentObject()?");

		return m_parent.u_object;
	}
	else
	{
		return nullptr;
	}
}

}// end namespace ph::editor
