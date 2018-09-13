#include "Core/Intersectable/PEmpty.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Core/Ray.h"

namespace ph
{

// HACK
bool PEmpty::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	if(ray.getMaxT() >= std::numeric_limits<real>::max())
	{
		probe.pushBaseHit(this, ray.getMaxT());
		return true;
	}

	return false;
}

// HACK
bool PEmpty::isIntersecting(const Ray& ray) const
{
	if(ray.getMaxT() >= std::numeric_limits<real>::max())
	{
		return true;
	}

	return false;
}

void PEmpty::calcIntersectionDetail(
	const Ray& ray,
	HitProbe&  probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	out_detail->setMisc(this, {0, 0, 0}, probe.getHitRayT());

	// HACK
	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(ray.getOrigin() + ray.getDirection() * probe.getHitRayT(), ray.getDirection().mul(-1), ray.getDirection().mul(-1));
	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
}

}// end namespace ph