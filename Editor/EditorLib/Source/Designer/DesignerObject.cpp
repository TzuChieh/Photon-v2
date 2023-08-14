#include "DesignerObject.h"
#include "Designer/DesignerScene.h"

#include <Common/assertion.h>
#include <Utility/utility.h>
#include <Utility/exception.h>
#include <Math/TVector3.h>
#include <Math/TQuaternion.h>

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
	: AbstractDesignerObject()
	, m_parent()
	, m_name()
{
	m_parent.u_object = nullptr;
}

DesignerObject::DesignerObject(const DesignerObject& other) = default;

DesignerObject::DesignerObject(DesignerObject&& other) noexcept = default;

DesignerObject::~DesignerObject()
{
	// Initializations must happen either in pairs or not happen at all
	PH_ASSERT(
		(getState().has(EObjectState::HasInitialized) && getState().has(EObjectState::HasUninitialized)) ||
		(getState().hasNo(EObjectState::HasInitialized) && getState().hasNo(EObjectState::HasUninitialized)));
	PH_ASSERT(
		(getState().has(EObjectState::HasRenderInitialized) && getState().has(EObjectState::HasRenderUninitialized)) ||
		(getState().hasNo(EObjectState::HasRenderInitialized) && getState().hasNo(EObjectState::HasRenderUninitialized)));
}

DesignerObject& DesignerObject::operator = (const DesignerObject& rhs) = default;

DesignerObject& DesignerObject::operator = (DesignerObject&& rhs) noexcept = default;

void DesignerObject::init()
{
	PH_ASSERT(getState().hasNo(EObjectState::HasInitialized));
}

void DesignerObject::uninit()
{
	PH_ASSERT(getState().hasNo(EObjectState::HasUninitialized));
}

void DesignerObject::renderInit(RenderThreadCaller& caller)
{
	PH_ASSERT(getState().hasNo(EObjectState::HasRenderInitialized));
}

void DesignerObject::renderUninit(RenderThreadCaller& caller)
{
	PH_ASSERT(getState().hasNo(EObjectState::HasRenderUninitialized));
}

void DesignerObject::update(const MainThreadUpdateContext& ctx)
{}

void DesignerObject::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{}

void DesignerObject::createRenderCommands(RenderThreadCaller& caller)
{}

void DesignerObject::editorTranslate(const math::Vector3R& amount)
{
	auto transform = getLocalToParent();
	transform.translate(amount);

	setLocalToParent(transform);
}

void DesignerObject::editorRotate(const math::QuaternionR& additionalRotation)
{
	auto transform = getLocalToParent();
	transform.rotate(additionalRotation);

	setLocalToParent(transform);
}

void DesignerObject::editorScale(const math::Vector3R& amount)
{
	auto transform = getLocalToParent();
	transform.scale(amount);

	setLocalToParent(transform);
}

void DesignerObject::selected()
{}

void DesignerObject::deselected()
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
		// Recursively remove all children
		childObj->deleteAllChildren();
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

void DesignerObject::deleteAllChildren()
{
	if(!canHaveChildren())
	{
		return;
	}

	// Remove all children, starting from the last child
	while(haveChildren())
	{
		deleteChild(getChildren().back());
	}
}

void DesignerObject::setName(std::string name)
{
	// TODO: ensure unique name in scene
	m_name = std::move(name);
}

void DesignerObject::select()
{
	if(getScene().selectObject(this))
	{
		selected();
	}
}

void DesignerObject::deselect()
{
	if(getScene().deselectObject(this))
	{
		deselected();
	}
}

void DesignerObject::setVisibility(const bool isVisible)
{
	getScene().changeObjectVisibility(this, isVisible);
}

void DesignerObject::setTick(const bool shouldTick)
{
	getScene().changeObjectTick(this, shouldTick);
}

void DesignerObject::setRenderTick(const bool shouldTick)
{
	getScene().changeObjectRenderTick(this, shouldTick);
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
