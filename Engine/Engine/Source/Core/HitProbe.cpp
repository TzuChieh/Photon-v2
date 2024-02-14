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
