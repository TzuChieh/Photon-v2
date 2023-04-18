#pragma once

#include "Designer/DesignerObject.h"
#include "Designer/DesignerScene.h"

#include <Utility/traits.h>

#include <utility>

namespace ph::editor
{

template<typename ChildType, typename... DeducedArgs>
inline ChildType* DesignerObject::initNewChild(DeducedArgs&&... args)
{
	static_assert(CDerived<ChildType, DesignerObject>,
		"Child type must be a designer object.");

	PH_ASSERT(m_scene != nullptr);

	ChildType* child = getScene().initNewObject<ChildType>(std::forward<DeducedArgs>(args)...);
	m_children.push_back(child);
	return m_children.back();
}

inline auto DesignerObject::getState() const
-> const TEnumFlags<EObjectState>&
{
	return m_state;
}

inline auto DesignerObject::getState()
-> TEnumFlags<EObjectState>&
{
	return m_state;
}

inline DesignerScene& DesignerObject::getScene()
{
	PH_ASSERT(m_scene);
	return *m_scene;
}

inline const DesignerScene& DesignerObject::getScene() const
{
	PH_ASSERT(m_scene);
	return *m_scene;
}

inline const std::string& DesignerObject::getName() const
{
	return m_name;
}

inline DesignerObject* DesignerObject::getChild(const std::size_t childIndex) const
{
	return childIndex < m_children.size() ? m_children[childIndex] : nullptr;
}

inline std::size_t DesignerObject::numChildren() const
{
	return m_children.size();
}

inline bool DesignerObject::hasChildren() const
{
	return numChildren() > 0;
}

}// end namespace ph::editor
