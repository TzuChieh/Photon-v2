#include "Designer/Render/RenderAgent.h"
#include "Designer/Render/RenderConfig.h"

namespace ph::editor
{

math::TDecomposedTransform<real> RenderAgent::getLocalToParent() const
{
	return m_agentTransform.getDecomposed();
}

void RenderAgent::setLocalToParent(const math::TDecomposedTransform<real>& transform)
{
	m_agentTransform.set(transform);
}

void RenderAgent::renderWithDefaultConfig()
{
	render(getRenderConfig());
}

}// end namespace ph::editor
