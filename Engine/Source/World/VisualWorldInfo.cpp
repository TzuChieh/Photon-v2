#include "World/VisualWorldInfo.h"

namespace ph
{

auto VisualWorldInfo::getRootActorsBound() const
	-> AABB3D
{
	return m_rootActorsBound;
}

void VisualWorldInfo::setRootActorsBound(const AABB3D& bound)
{
	m_rootActorsBound = bound;
}

}// end namespace ph