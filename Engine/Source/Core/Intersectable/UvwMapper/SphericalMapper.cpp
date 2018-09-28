#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Math/TVector3.h"
#include "Math/math.h"
#include "Math/constant.h"

#include <iostream>
#include <cmath>

namespace ph
{

void SphericalMapper::directionToUvw(const Vector3R& direction, Vector3R* const out_uvw) const
{
	PH_ASSERT(out_uvw);

	const Vector3R& unitVector = direction.normalize();

	const real cosTheta = math::clamp(unitVector.y, -1.0_r, 1.0_r);

	const real theta  = std::acos(cosTheta);                                   // [  0,   pi]
	const real phiRaw = std::atan2(unitVector.x, unitVector.z);                // [-pi,   pi]
	const real phi    = phiRaw >= 0.0_r ? phiRaw : 2.0_r * PH_PI_REAL + phiRaw;// [  0, 2*pi]

	out_uvw->x = phi / (2.0_r * PH_PI_REAL);       // [0, 1]
	out_uvw->y = (PH_PI_REAL - theta) / PH_PI_REAL;// [0, 1]
	out_uvw->z = 0.0_r;
}

bool SphericalMapper::uvwToDirection(const Vector3R& uvw, Vector3R* const out_direction) const
{
	PH_ASSERT(out_direction);

	PH_ASSERT(0.0_r <= uvw.x && uvw.x <= 1.0_r &&
	          0.0_r <= uvw.y && uvw.y <= 1.0_r);

	const real theta = (1.0_r - uvw.y) * PH_PI_REAL;
	const real phi   = uvw.x * PH_PI_REAL * 2.0_r;

	const real zxPlaneRadius = std::sin(theta);
	out_direction->x = zxPlaneRadius * std::sin(phi);
	out_direction->y = std::cos(theta);
	out_direction->z = zxPlaneRadius * std::cos(phi);

	return true;
}

}// end namespace ph