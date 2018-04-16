#include "World/VisualWorldInfo.h"

namespace ph
{

AABB3D VisualWorldInfo::getRootActorsBound() const
{
	return m_rootActorsBound;
}

void VisualWorldInfo::setRootActorsBound(const AABB3D& bound)
{
	m_rootActorsBound = bound;
}

}// end namespace ph