#pragma once

#include "Engine/Core/hit_fwd.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/math.h"
#include "Engine/Utility/TBitFlags.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

class Primitive;
class SurfaceOptics;
class VolumeOptics;
class Emitter;

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

using SurfaceHitReason = TEnumFlags<ESurfaceHitReason>;

/*! @brief General information about a ray-surface intersection event.
*/
class SurfaceHit final
{
public:
	/*! @brief Empty event. Nothing is hit.
	*/
	SurfaceHit();

	/*! @brief Construct from the ray and probe involved in a hit event.
	A full hit detail will be computed. If this is undesirable (e.g., full hit detail is not required),
	use the overload which let you set the hit detail directly.
	*/
	SurfaceHit(
		const Ray&       ray,
		const HitProbe&  probe,
		SurfaceHitReason reason);

	SurfaceHit(
		const Ray&       ray,
		const HitProbe&  probe,
		const HitDetail& detail,
		SurfaceHitReason reason);

	SurfaceHit switchChannel(uint32 newChannel) const;

	/*! @brief Intersect the intersected object again with a different ray.
	@param ray The different ray to use for intersection test.
	@param probe The probe to record the intersection.
	@note Generates hit event (with `ray` and `probe`).
	*/
	bool reintersect(const Ray& ray, HitProbe& probe) const;

	bool hasSurfaceOptics() const;
	bool hasInteriorOptics() const;
	bool hasExteriorOptics() const;

	const HitDetail& getDetail() const;
	SurfaceHitReason getReason() const;

	/*!
	@return The ray that caused a hit event.
	*/
	const Ray& getRay() const;

	/*! @brief Convenient method for `getRay()` where `getReason()` contains `ESurfaceHitReason::IncidentRay`. 
	*/
	const Ray& getIncidentRay() const;

	const Time& getTime() const;
	math::Vector3R getPos() const;
	math::Vector3R getShadingNormal() const;
	math::Vector3R getGeometryNormal() const;

	const Emitter* getSurfaceEmitter() const;
	const SurfaceOptics* getSurfaceOptics() const;
	const VolumeOptics* getInteriorOptics() const;
	const VolumeOptics* getExteriorOptics() const;

private:
	Ray              m_ray;
	HitProbe         m_recordedProbe;
	HitDetail        m_detail;
	SurfaceHitReason m_reason;
};

// In-header Implementations:

inline SurfaceHit::SurfaceHit()
	: m_ray          {}
	, m_recordedProbe{}
	, m_detail       {}
	, m_reason       {}
{}

inline SurfaceHit::SurfaceHit(
	const Ray&       ray,
	const HitProbe&  probe,
	SurfaceHitReason reason)

	: m_ray          {ray}
	, m_recordedProbe{probe}
	, m_detail       {}
	, m_reason       {reason}
{
	probe.calcFullHitDetail(ray, &m_detail);
}

inline SurfaceHit::SurfaceHit(
	const Ray&             ray, 
	const HitProbe&        probe, 
	const HitDetail&       detail,
	const SurfaceHitReason reason)

	: m_ray          {ray}
	, m_recordedProbe{probe}
	, m_detail       {detail}
	, m_reason       {reason}
{}

inline bool SurfaceHit::reintersect(const Ray& ray, HitProbe& probe) const
{
	return m_recordedProbe.reintersect(ray, probe, getRay());
}

inline const HitDetail& SurfaceHit::getDetail() const
{
	return m_detail;
}

inline SurfaceHitReason SurfaceHit::getReason() const
{
	return m_reason;
}

inline const Ray& SurfaceHit::getRay() const
{
	PH_ASSERT(!m_reason.hasExactly(ESurfaceHitReason::Invalid));
	return m_ray;
}

inline const Ray& SurfaceHit::getIncidentRay() const
{
	PH_ASSERT(m_reason.has(ESurfaceHitReason::IncidentRay));
	return getRay();
}

inline const Time& SurfaceHit::getTime() const
{
	return m_ray.getTime();
}

inline math::Vector3R SurfaceHit::getPos() const
{
	return m_detail.getPos();
}

inline math::Vector3R SurfaceHit::getShadingNormal() const
{
	return m_detail.getShadingNormal();
}

inline math::Vector3R SurfaceHit::getGeometryNormal() const
{
	return m_detail.getGeometryNormal();
}

}// end namespace ph
