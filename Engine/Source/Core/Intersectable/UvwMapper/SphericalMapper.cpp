#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Math/TVector3.h"
#include "Math/math.h"
#include "Math/constant.h"
#include "Math/TOrthonormalBasis3.h"

#include <iostream>
#include <cmath>

namespace ph
{

void SphericalMapper::positionToUvw(const Vector3R& position, Vector3R* const out_uvw) const
{
	if(position.lengthSquared() < 1e-8)
	{
		out_uvw->set(0, 0, 0);
		return;
	}

	const Vector3R& unitVector = position.normalize();

	const real cosTheta = math::clamp(unitVector.y, -1.0_r, 1.0_r);

	const real theta  = std::acos(cosTheta);                                   // [  0,   pi]
	const real phiRaw = std::atan2(unitVector.x, unitVector.z);                // [-pi,   pi]
	const real phi    = phiRaw >= 0.0_r ? phiRaw : 2.0_r * PH_PI_REAL + phiRaw;// [  0, 2*pi]

	out_uvw->x = phi / (2.0_r * PH_PI_REAL);       // [0, 1]
	out_uvw->y = (PH_PI_REAL - theta) / PH_PI_REAL;// [0, 1]
	out_uvw->z = 0.0_r;
}

}// end namespace ph