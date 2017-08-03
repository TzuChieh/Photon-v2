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

bool RendererProxy::getNewWork(RenderWork* out_work)
{
	return m_renderer->getNewWork(out_work);
}

void RendererProxy::submitWork(const RenderWork& work, bool isUpdating)
{
	m_renderer->submitWork(work, isUpdating);
}

}// end namespace ph