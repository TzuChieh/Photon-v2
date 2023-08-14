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
	/*math::TDecomposedTransform<real> getLocalToParent() const override;
	void setLocalToParent(const math::TDecomposedTransform<real>& transform) override;*/

private:
	ObjectTransform m_nodeTransform;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<NodeObject>)
	{
		ClassType clazz("node-dobj");
		clazz.docName("Node Designer Object");
		clazz.description("Node that connects objects to form a hierarchy.");
		clazz.baseOn<HierarchicalDesignerObject>();

		clazz.addStruct(&OwnerType::m_nodeTransform);

		return clazz;
	}
};

}// end namespace ph::editor
