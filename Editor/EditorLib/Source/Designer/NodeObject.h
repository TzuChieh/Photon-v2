#pragma once

#include "Designer/HierarchicalDesignerObject.h"
#include "Designer/Basic/ObjectTransform.h"

#include <SDL/sdl_interface.h>

#include <string>

namespace ph::editor
{

class NodeObject : public HierarchicalDesignerObject
{
public:

private:
	ObjectTransform m_transform;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<NodeObject>)
	{
		ClassType clazz("node-dobj");
		clazz.docName("Node Designer Object");
		clazz.description("Node that connects objects to form a hierarchy.");
		clazz.baseOn<HierarchicalDesignerObject>();

		clazz.addStruct(&OwnerType::m_transform);

		return clazz;
	}
};

}// end namespace ph::editor
