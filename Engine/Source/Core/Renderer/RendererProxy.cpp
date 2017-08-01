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

void RendererProxy::asyncAddUpdatedRegion(const Region& region)
{
	m_renderer->asyncAddUpdatedRegion(region);
}

}// end namespace ph