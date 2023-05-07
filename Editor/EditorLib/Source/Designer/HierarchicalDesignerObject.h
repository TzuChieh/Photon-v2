#pragma once

#include "Designer/DesignerObject.h"

#include <SDL/sdl_interface.h>

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

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<HierarchicalDesignerObject>)
	{
		ClassType clazz("hierarchical-dobj");
		clazz.docName("Hierarchical Designer Object");
		clazz.description("A designer object that allow children.");
		clazz.baseOn<DesignerObject>();
		return clazz;
	}
};

}// end namespace ph::editor

#include "Designer/HierarchicalDesignerObject.ipp"
