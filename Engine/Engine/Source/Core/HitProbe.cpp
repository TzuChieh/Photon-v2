#include "Core/HitProbe.h"
#include "Core/Intersection/PrimitiveMetadata.h"
#include "Core/Intersection/Intersectable.h"
#include "Core/HitDetail.h"

#include <Common/assertion.h>

#include <type_traits>

namespace ph
{

// A simple value type should be trivially copyable
static_assert(std::is_trivially_copyable_v<HitProbe>);

void HitProbe::calcHitDetail(
	const Ray&       ray,
	HitDetail* const out_detail)
{
	PH_ASSERT(getCurrentHit() != nullptr);

	getCurrentHit()->calcHitDetail(ray, *this, out_detail);
}

void HitProbe::calcFullHitDetail(
	const Ray&       ray,
	HitDetail* const out_detail)
{
	calcHitDetail(ray, out_detail);
	out_detail->computeBases();
}

bool HitProbe::reintersect(const Ray& ray, HitProbe& probe, const Ray& srcRay)
{
	PH_ASSERT(getCurrentHit() != nullptr);

	return getCurrentHit()->reintersect(ray, probe, srcRay, *this);
}

bool HitProbe::isOnDefaultChannel() const
{
	return m_hitDetailChannel == PrimitiveMetadata::DEFAULT_CHANNEL_ID;
}

}// end namespace ph
