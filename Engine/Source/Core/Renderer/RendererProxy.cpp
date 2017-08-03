#include "Core/Renderer/RendererProxy.h"
#include "Core/Renderer/Renderer.h"

namespace ph
{

RendererProxy::RendererProxy() :
	RendererProxy(nullptr)
{

}

RendererProxy::RendererProxy(Renderer* renderer) :
	m_renderer(renderer)
{

}

bool RendererProxy::getNewWork(const uint32 workerId, RenderWork* out_work)
{
	return m_renderer->getNewWork(workerId, out_work);
}

void RendererProxy::submitWork(const uint32 workerId, const RenderWork& work, bool isUpdating)
{
	m_renderer->submitWork(workerId, work, isUpdating);
}

}// end namespace ph