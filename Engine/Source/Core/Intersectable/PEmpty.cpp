#include "Core/Intersectable/PEmpty.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"

namespace ph
{

void PEmpty::calcIntersectionDetail(
	const Ray& /* ray */,
	HitProbe&  probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	out_detail->setMisc(this, {0, 0, 0}, probe.getHitRayT());
}

}// end namespace ph