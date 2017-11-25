#include "Actor/TextureMapper/SphericalMapper.h"
#include "Math/TVector3.h"
#include "Math/Math.h"
#include "Math/constant.h"

#include <iostream>

namespace ph
{

SphericalMapper::~SphericalMapper() = default;

void SphericalMapper::map(const Vector3R& position, const Vector3R& uvw, Vector3R* const out_uvw) const
{
	if(position.length() < 1e-8)
	{
		std::cerr << "warning: at SphericalMapper::map(), positions too close to geometry origin may induce errors during mapping" << std::endl;
	}

	const Vector3R positionDir = position.normalize();

	const real cosTheta = Math::clamp(positionDir.y, -1.0_r, 1.0_r);
	const real cosPhi   = Math::clamp(positionDir.x, -1.0_r, 1.0_r);
	const real sinPhi   = Math::clamp(positionDir.z, -1.0_r, 1.0_r);

	const real theta = acos(cosTheta);       // [  0, pi]
	const real phi   = atan2(sinPhi, cosPhi);// [-pi, pi]

	out_uvw->x = (-phi + PH_PI_REAL) / (2.0_r * PH_PI_REAL);// [0, 1]
	out_uvw->y = (PH_PI_REAL - theta) / PH_PI_REAL;         // [0, 1]
	out_uvw->z = 0.0_r;
}

}// end namespace ph