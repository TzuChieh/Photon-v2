#include "Designer/Render/RenderAgent.h"

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

}// end namespace ph::editor
