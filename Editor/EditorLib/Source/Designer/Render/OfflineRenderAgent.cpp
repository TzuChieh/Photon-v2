#include "Designer/Render/OfflineRenderAgent.h"
#include "Designer/DesignerScene.h"
#include "Render/RenderThreadCaller.h"
#include "Render/Renderer/OfflineRenderer.h"
#include "Render/Scene.h"

#include <utility>
#include <memory>

namespace ph::editor
{

math::TDecomposedTransform<real> OfflineRenderAgent::getLocalToParent() const
{
	return m_agentTransform.getDecomposed();
}

void OfflineRenderAgent::setLocalToParent(const math::TDecomposedTransform<real>& transform)
{
	m_agentTransform.set(transform);
}

void OfflineRenderAgent::renderInit(RenderThreadCaller& caller)
{
	Base::renderInit(caller);

	auto renderer = std::make_unique<render::OfflineRenderer>();
	m_renderer = renderer.get();
	caller.add(
		[&scene = getScene().getRendererScene(), renderer = std::move(renderer)](render::System& /* sys */) mutable
		{
			scene.addDynamicResource(std::move(renderer));
		});

	getScene().addRendererBinding({
		.ownerObj = this,
		.offlineRenderer = m_renderer});
}

void OfflineRenderAgent::renderUninit(RenderThreadCaller& caller)
{
	getScene().removeRendererBinding(this);

	caller.add(
		[&scene = getScene().getRendererScene(), renderer = m_renderer](render::System& /* sys */)
		{
			scene.removeResource(renderer);
		});
	m_renderer = nullptr;

	Base::renderUninit(caller);
}

void OfflineRenderAgent::update(const MainThreadUpdateContext& ctx)
{
	Base::update(ctx);
}

void OfflineRenderAgent::renderUpdate(const MainThreadRenderUpdateContext& ctx)
{
	Base::renderUpdate(ctx);
}

}// end namespace ph::editor
