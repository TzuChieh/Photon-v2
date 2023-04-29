#pragma once

#include "Designer/DesignerObject.h"

#include <vector>

namespace ph::editor
{

class HierarchicalDesignerObject : public DesignerObject
{
public:
	HierarchicalDesignerObject();

	TSpanView<DesignerObject*> getChildren() const override;
	bool canHaveChildren() const override;

private:
	DesignerObject* addChild(DesignerObject* childObj) override;
	bool removeChild(DesignerObject* childObj) override;

private:
	std::vector<DesignerObject*> m_children;
};

}// end namespace ph::editor

#include "Designer/HierarchicalDesignerObject.ipp"
