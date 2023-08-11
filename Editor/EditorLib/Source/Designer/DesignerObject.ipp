#pragma once

#include "Designer/DesignerObject.h"
#include "Designer/DesignerScene.h"

#include <Utility/traits.h>

#include <utility>

namespace ph::editor
{

template<typename ChildType>
inline ChildType* DesignerObject::newChild(
	const bool shouldInit,
	const bool shouldSetToDefault)
{
	static_assert(CDerived<ChildType, DesignerObject>,
		"Child type must be a designer object.");

	if(!canHaveChildren())
	{
		return nullptr;
	}

	ChildType* childObj = newChild(
		ChildType::getSdlClass(),
		shouldInit,
		shouldSetToDefault);

	// We know the exact type
	return static_cast<ChildType*>(childObj);
}

inline bool DesignerObject::isSelected() const
{
	return getState().has(EObjectState::Selected);
}

inline bool DesignerObject::isVisible() const
{
	return getState().hasNo(EObjectState::Hidden);
}

inline void DesignerObject::setParentObject(DesignerObject* const object)
{
	PH_ASSERT(object != nullptr);
	PH_ASSERT(object != this);

	// Only root object can have a scene as parent
	PH_ASSERT(getState().hasNo(EObjectState::Root));

	m_parent.u_object = object;
}

inline void DesignerObject::setParentScene(DesignerScene* const scene)
{
	PH_ASSERT(scene != nullptr);

	// Only root object can have a scene as parent
	PH_ASSERT(getState().has(EObjectState::Root));

	m_parent.u_scene = scene;
}

inline const std::string& DesignerObject::getName() const
{
	return m_name;
}

inline bool DesignerObject::haveChildren() const
{
	return getChildren().size() > 0;
}

}// end namespace ph::editor
