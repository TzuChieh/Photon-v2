#include "Core/Intersectable/UvwMapper/SphericalMapper.h"
#include "Math/TVector3.h"
#include "Math/Math.h"
#include "Math/constant.h"
#include "Math/TOrthonormalBasis3.h"

#include <iostream>
#include <cmath>

namespace ph
{

SphericalMapper::~SphericalMapper() = default;

void SphericalMapper::map(const Vector3R& position, Vector3R* const out_uvw) const
{
	if(position.length() < 1e-8)
	{
		std::cerr << "warning: at SphericalMapper::map(), "
		          << "positions too close to origin may induce errors during mapping" << std::endl;
	}

	const Vector3R positionDir = position.normalize();

	const real cosTheta = Math::clamp(positionDir.y, -1.0_r, 1.0_r);

	const real theta  = std::acos(cosTheta);                                   // [  0,   pi]
	const real phiRaw = std::atan2(positionDir.x, positionDir.z);              // [-pi,   pi]
	const real phi    = phiRaw >= 0.0_r ? phiRaw : 2.0_r * PH_PI_REAL + phiRaw;// [  0, 2*pi]

	out_uvw->x = phi / (2.0_r * PH_PI_REAL);       // [0, 1]
	out_uvw->y = (PH_PI_REAL - theta) / PH_PI_REAL;// [0, 1]
	out_uvw->z = 0.0_r;
}

}// end namespace ph