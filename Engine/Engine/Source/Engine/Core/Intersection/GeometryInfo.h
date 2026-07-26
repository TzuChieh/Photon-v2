#pragma once

#include "Engine/Math/Geometry/TAABB3D.h"

namespace ph
{

/*! @brief Geometry-wide information.
*/
struct GeometryInfo final
{
	math::AABB3D localAABB = math::AABB3D::makeEmpty();
};

}// end namespace ph
