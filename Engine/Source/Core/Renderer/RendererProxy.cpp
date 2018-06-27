#include "Core/Renderer/RendererProxy.h"
#include "Core/Renderer/Renderer.h"
#include "Core/Renderer/RenderWorker.h"

namespace ph
{

RendererProxy::RendererProxy() :
	RendererProxy(nullptr)
{}

RendererProxy::RendererProxy(Renderer* renderer) :
	m_renderer(renderer)
{}

bool RendererProxy::supplyWork(RenderWorker& worker)
{
	return m_renderer->asyncSupplyWork(worker);
}

void RendererProxy::submitWork(RenderWorker& worker)
{
	m_renderer->asyncSubmitWork(worker);
}

}// end namespace ph