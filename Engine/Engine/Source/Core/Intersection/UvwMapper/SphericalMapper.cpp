#include "Core/Intersection/UvwMapper/SphericalMapper.h"
#include "Math/TVector3.h"
#include "Math/math.h"
#include "Math/constant.h"

#include <cmath>

namespace ph
{

void SphericalMapper::dirToUvw(const math::Vector3R& dir, math::Vector3R* const out_uvw) const
{
	using namespace math::constant;

	PH_ASSERT(out_uvw);

	const math::Vector3R& unitVector = dir.normalize();

	const real cosTheta = math::clamp(unitVector.y(), -1.0_r, 1.0_r);

	const real theta  = std::acos(cosTheta);                             // [  0,   pi]
	const real phiRaw = std::atan2(unitVector.x(), unitVector.z());      // [-pi,   pi]
	const real phi    = phiRaw >= 0.0_r ? phiRaw : two_pi<real> + phiRaw;// [  0, 2*pi]

	out_uvw->x() = phi / two_pi<real>;           // [0, 1]
	out_uvw->y() = (pi<real> - theta) / pi<real>;// [0, 1]
	out_uvw->z() = 0.0_r;
}

bool SphericalMapper::uvwToDir(const math::Vector3R& uvw, math::Vector3R* const out_dir) const
{
	PH_ASSERT(out_dir);

	PH_ASSERT(0.0_r <= uvw.x() && uvw.x() <= 1.0_r &&
	          0.0_r <= uvw.y() && uvw.y() <= 1.0_r);

	const real theta = (1.0_r - uvw.y()) * math::constant::pi<real>;
	const real phi   = uvw.x() * math::constant::two_pi<real>;

	const real zxPlaneRadius = std::sin(theta);
	out_dir->x() = zxPlaneRadius * std::sin(phi);
	out_dir->y() = std::cos(theta);
	out_dir->z() = zxPlaneRadius * std::cos(phi);

	return true;
}

}// end namespace ph
