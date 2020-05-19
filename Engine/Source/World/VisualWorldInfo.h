#pragma once

#include "Math/Geometry/TAABB3D.h"

namespace ph
{

/*! @brief This class gathers information about visual world such as world bounds.
*/
class VisualWorldInfo final
{
public:
	VisualWorldInfo();

	/*! @brief Bounds actors cooked in the first level.

	The bound is only available after the first level has done cooking.
	*/
	math::AABB3D getRootActorsBound() const;

	/*! @brief Bounds actors from levels finished cooking.

	The bound is updated every time a level has done cooking.
	*/
	math::AABB3D getLeafActorsBound() const;

	void setRootActorsBound(const math::AABB3D& bound);
	void setLeafActorsBound(const math::AABB3D& bound);

private:
	math::AABB3D m_rootActorsBound;
	math::AABB3D m_leafActorsBound;
};

// In-header Implementations:

inline VisualWorldInfo::VisualWorldInfo() : 
	m_rootActorsBound(math::Vector3R(0)),
	m_leafActorsBound(math::Vector3R(0))
{}

inline math::AABB3D VisualWorldInfo::getRootActorsBound() const
{
	return m_rootActorsBound;
}

inline math::AABB3D VisualWorldInfo::getLeafActorsBound() const
{
	return m_leafActorsBound;
}

inline void VisualWorldInfo::setRootActorsBound(const math::AABB3D& bound)
{
	m_rootActorsBound = bound;
}

inline void VisualWorldInfo::setLeafActorsBound(const math::AABB3D& bound)
{
	m_leafActorsBound = bound;
}

}// end namespace ph
