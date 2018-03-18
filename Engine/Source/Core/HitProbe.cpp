#include "Core/HitProbe.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Intersectable.h"
#include "Core/HitDetail.h"

namespace ph
{

void HitProbe::calcIntersectionDetail(
	const Ray&       ray,
	HitDetail* const out_detail)
{
	PH_ASSERT(getCurrentHit() != nullptr);

	getCurrentHit()->calcIntersectionDetail(ray, *this, out_detail);
	out_detail->computeBases();
}

}// end namespace ph