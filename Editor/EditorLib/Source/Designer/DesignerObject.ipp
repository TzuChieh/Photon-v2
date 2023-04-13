#pragma once

#include "Designer/DesignerObject.h"
#include "Designer/DesignerScene.h"

#include <Utility/traits.h>

#include <utility>

namespace ph::editor
{

template<typename ChildType, typename... DeducedArgs>
const std::shared_ptr<DesignerObject>& DesignerObject::createChild(DeducedArgs&&... args)
{
	static_assert(CDerived<ChildType, DesignerObject>,
		"Child type must be a designer object.");

	auto child = std::make_shared<ChildType>(std::forward<DeducedArgs>(args)...);
	if(m_scene != nullptr)
	{
		m_scene->onObjectCreated(child.get());
	}

	m_children.push_back(std::move(child));
	return m_children.back();
}

}// end namespace ph::editor
