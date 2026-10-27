#pragma once

#include "Engine/Core/hit_fwd.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Utility/TBitFlags.h"

#include <Common/assertion.h>

#include <utility>

namespace ph
{

class Intersectable;

enum class ESurfaceHitReason : detail::HitReasonIntType
{
	/*! Invalid state. Most likely the reason has not been set. */
	Invalid = 0,

	/*! An uncategorized, unknown reason. */
	Unknown = detail::hr_unknown_bits,

	/*! A ray has hit the surface. */
	IncidentRay = detail::hr_incident_ray_bits,

	/*! A position from the surface has been picked. */
	SampledPos = detail::hr_sampled_pos_bits,

	/*! A direction from the surface has been picked. */
	SampledDir = detail::hr_sampled_dir_bits,

	/*! Both a position and a direction from the surface are picked. */
	SampledPosDir = detail::hr_sampled_pos_bits | detail::hr_sampled_dir_bits,
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(ESurfaceHitReason);

using SurfaceHitReasons = TEnumFlags<ESurfaceHitReason>;

/*! @brief An owning record of a hit and its associated ray.
*/
class HitEvent final
{
public:
	HitEvent() = default;

	/*! @brief Create an empty event for a ray.
	Use `reset()` to prepare a used event for another intersection.
	*/
	explicit HitEvent(
		const Ray& ray,
		SurfaceHitReasons reason = ESurfaceHitReason::IncidentRay);

	/*! @brief Fork working state for another ray without copying the original ray.
	*/
	HitEvent(const Ray& ray, const HitEvent& source);

	/*! @brief Copy an event with a different ray segment.
	@param segment Replacement ray segment.
	@param source Supplies the time, probe, and reason.
	*/
	HitEvent(const math::TLineSegment<real>& segment, const HitEvent& source);

	void reset(const Ray& ray, SurfaceHitReasons reason = ESurfaceHitReason::IncidentRay);

	/*! @brief Clear recorded hits while retaining the ray, channel, and reason.
	*/
	void clearHit();

	void pushBaseHit(const Intersectable* hitTarget, real hitRayT);
	void pushIntermediateHit(const Intersectable* hitTarget);
	void popHit();

	/*! @brief Accept a hit record, including its channel and reason, while preserving this ray.
	@param source Hit consistent with this event's ray (e.g., the same hit point in another space).
	*/
	void copyHitAndKeepRay(const HitEvent& source);

	/*! @brief Copy a hit record with a new parametetric hit distance, while preserving this ray.
	@param source Hit to copy.
	@param hitRayT New hit parametric distance for source event's ray.
	*/
	void copyHitAndKeepRay(const HitEvent& source, real hitRayT);

	/*! @brief Calculate basic hit information using a working copy of this event.
	Only basic hit information is calculated. For detailed hit information, there are dedicated
	methods such as `HitDetail::computeBases()` so they can be obtained on demand.
	*/
	void calcHitDetail(HitDetail* out_detail) const;

	/*! @brief Test a new ray against the hit targets recorded in this event.
	@param event Contains the new ray and receives the new hit. Must be ready for recording
	and must not be `*this`. Its ray is preserved.
	@return Whether a hit is found. On a miss, @p event's hit record is undefined.
	*/
	bool reintersect(HitEvent& event) const;

	const Ray& getRay() const;
	const Intersectable* getTopHit() const;

	/*!
	@return Recorded hit distance, or `std::numeric_limits<real>::max()` for no hit.
	Construction, `reset()`, and `clearHit()` set the no-hit value.
	*/
	real getHitRayT() const;

	uint8 getChannel() const;
	SurfaceHitReasons getReason() const;
	bool hasRecordedTargets() const;

	/*! @brief Access the embedded probe.
	*/
	///@{
	const HitProbe& getProbe() const;
	HitProbe& probe();
	///@}

	void setChannel(uint8 channel);
	void setReason(SurfaceHitReasons reason);

	/*! @brief Change the working ray.
	@param ray New ray, consistent with the recorded hit (e.g., the same hit point in another space).
	*/
	void setRay(const Ray& ray);

	/*! @brief Temporarily replace the ray while retaining all hit writes.
	@param ray Temporary ray whose hit records remain consistent with the original ray
	(e.g., the same hit point in another space).
	@param func Operation on this event. The original ray is restored on return or exception.
	*/
	template<typename Func>
	decltype(auto) withRay(const Ray& ray, Func&& func);

private:
	Ray               m_ray;
	HitProbe          m_probe;
	SurfaceHitReasons m_reason;
};

// In-header Implementations:

inline HitEvent::HitEvent(const Ray& ray, const SurfaceHitReasons reason)
	: m_ray(ray), m_reason(reason)
{}

inline HitEvent::HitEvent(const Ray& ray, const HitEvent& source)
	: m_ray(ray), m_probe(source.m_probe), m_reason(source.m_reason)
{}

inline HitEvent::HitEvent(const math::TLineSegment<real>& segment, const HitEvent& source)
	: m_ray(segment, source.m_ray.getTime())
	, m_probe(source.m_probe)
	, m_reason(source.m_reason)
{}

inline void HitEvent::reset(const Ray& ray, const SurfaceHitReasons reason)
{
	setRay(ray);
	m_probe.clearHit();
	m_probe.setChannel(0);
	setReason(reason);
}

inline void HitEvent::clearHit()
{
	m_probe.clearHit();
}

inline void HitEvent::pushBaseHit(const Intersectable* const hitTarget, const real hitRayT)
{
	m_probe.pushBaseHit(hitTarget, hitRayT);
}

inline void HitEvent::pushIntermediateHit(const Intersectable* const hitTarget)
{
	m_probe.pushIntermediateHit(hitTarget);
}

inline void HitEvent::popHit()
{
	m_probe.popHit();
}

inline void HitEvent::copyHitAndKeepRay(const HitEvent& source)
{
	PH_ASSERT(source.hasRecordedTargets());
	m_probe = source.m_probe;
	m_reason = source.m_reason;
}

inline void HitEvent::copyHitAndKeepRay(const HitEvent& source, const real hitRayT)
{
	copyHitAndKeepRay(source);
	m_probe.replaceBaseHitRayT(hitRayT);
}

inline const Ray& HitEvent::getRay() const
{
	return m_ray;
}

inline const Intersectable* HitEvent::getTopHit() const
{
	return m_probe.getTopHit();
}

inline real HitEvent::getHitRayT() const
{
	return m_probe.getHitRayT();
}

inline uint8 HitEvent::getChannel() const
{
	return m_probe.getChannel();
}

inline SurfaceHitReasons HitEvent::getReason() const
{
	return m_reason;
}

inline bool HitEvent::hasRecordedTargets() const
{
	return m_probe.hasRecordedTargets();
}

inline const HitProbe& HitEvent::getProbe() const
{
	return m_probe;
}

inline HitProbe& HitEvent::probe()
{
	return m_probe;
}

inline void HitEvent::setChannel(const uint8 channel)
{
	m_probe.setChannel(channel);
}

inline void HitEvent::setReason(const SurfaceHitReasons reason)
{
	m_reason = reason;
}

inline void HitEvent::setRay(const Ray& ray)
{
	m_ray = ray;
}

template<typename Func>
inline decltype(auto) HitEvent::withRay(const Ray& ray, Func&& func)
{
	// RAII guard that restors original ray
	struct RayRestorer
	{
		Ray& target;
		const Ray original;

		~RayRestorer()
		{
			target = original;
		}
	};

	RayRestorer restoreRay{m_ray, m_ray};
	setRay(ray);
	return std::forward<Func>(func)(*this);
}

}// end namespace ph
