#pragma once

#include "Designer/FlatDesignerObject.h"
#include "Designer/Basic/ObjectTransform.h"

#include <SDL/sdl_interface.h>

#include <string>

namespace ph::editor::render { class OfflineRenderer; }

namespace ph::editor
{

class OfflineRenderAgent : public FlatDesignerObject
{
public:
	using Base = FlatDesignerObject;

	math::TDecomposedTransform<real> getLocalToParent() const override;
	void setLocalToParent(const math::TDecomposedTransform<real>& transform) override;

	void renderInit(RenderThreadCaller& caller);
	void renderUninit(RenderThreadCaller& caller);
	void update(const MainThreadUpdateContext& ctx);
	void renderUpdate(const MainThreadRenderUpdateContext& ctx);

private:
	render::OfflineRenderer* m_renderer = nullptr;

	// SDL-binded fields
	ObjectTransform m_agentTransform;

public:
	PH_DEFINE_SDL_CLASS(TSdlOwnerClass<OfflineRenderAgent>)
	{
		ClassType clazz("offline-render-agent");
		clazz.docName("Offline Render Agent");
		clazz.description("Controls offline rendering.");
		clazz.baseOn<FlatDesignerObject>();

		clazz.addStruct(&OwnerType::m_agentTransform);

		return clazz;
	}
};

}// end namespace ph::editor
