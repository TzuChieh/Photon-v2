#include "Designer/HierarchicalDesignerObject.h"

namespace ph::editor
{

HierarchicalDesignerObject::HierarchicalDesignerObject()
	: DesignerObject()
	, m_children()
{}

TSpanView<DesignerObject*> HierarchicalDesignerObject::getChildren() const
{
	return m_children;
}

bool HierarchicalDesignerObject::canHaveChildren() const
{
	return true;
}

DesignerObject* HierarchicalDesignerObject::addChild(DesignerObject* const childObj)
{
	m_children.push_back(childObj);
	return childObj;
}

bool HierarchicalDesignerObject::removeChild(DesignerObject* const childObj)
{
	const auto numErasedObjs = std::erase(m_children, childObj);
	return numErasedObjs > 0;
}

}// end namespace ph::editor
