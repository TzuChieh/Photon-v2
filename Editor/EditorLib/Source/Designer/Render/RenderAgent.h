#pragma once

#include "Designer/FlatDesignerObject.h"
#include "Designer/Basic/ObjectTransform.h"

#include <SDL/sdl_interface.h>

namespace ph::editor::render { class OfflineRenderer; }

namespace ph::editor
{

class RenderConfig;

class RenderAgent : public FlatDesignerObject
{
public:
	virtual RenderConfig getRenderConfig() const = 0;

	math::TDecomposedTransform<real> getLocalToParent() const override;
	void setLocalToParent(const math::TDecomposedTransform<real>& transform) override;

private:
	// SDL-binded fields
	ObjectTransform m_agentTransform;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<RenderAgent>)
	{
		ClassType clazz("render-agent");
		clazz.docName("Render Agent");
		clazz.description("Controls rendering.");
		clazz.baseOn<FlatDesignerObject>();

		clazz.addStruct(&OwnerType::m_agentTransform);

		return clazz;
	}
};

}// end namespace ph::editor
