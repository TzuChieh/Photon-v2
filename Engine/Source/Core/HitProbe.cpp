#include "Core/HitProbe.h"
#include "Common/assertion.h"
#include "Core/Intersectable/Intersectable.h"
#include "Core/HitDetail.h"
#include "Core/Intersectable/PrimitiveMetadata.h"

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

bool HitProbe::isOnDefaultChannel() const
{
	return m_hitDetailChannel == PrimitiveMetadata::DEFAULT_CHANNEL_ID;
}

}// end namespace ph