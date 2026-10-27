#include "Engine/Core/HitEvent.h"
#include "Engine/Core/Intersection/Intersectable.h"

#include <type_traits>

namespace ph
{

static_assert(std::is_trivially_copyable_v<HitEvent>);

void HitEvent::calcHitDetail(HitDetail* const out_detail) const
{
	PH_ASSERT(hasRecordedTargets());
	HitEvent copiedEvent(*this);
	m_probe.getTopHit()->calcHitDetail(copiedEvent, out_detail);
}

bool HitEvent::reintersect(HitEvent& event) const
{
	PH_ASSERT(&event != this);
	PH_ASSERT(hasRecordedTargets());
	HitEvent copiedEvent(*this);
	return m_probe.getTopHit()->reintersect(event, copiedEvent);
}

}// end namespace ph
