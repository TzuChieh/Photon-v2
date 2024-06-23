#pragma once

#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Ray.h"
#include "Math/math.h"
#include "Utility/TBitFlags.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

class SurfaceOptics;
class VolumeOptics;
class Emitter;

namespace detail
{

using SurfaceHitReasonIntType = uint8;

inline constexpr auto shr_unknown_bits      = math::flag_bit<uint8, 0>();
inline constexpr auto shr_incident_ray_bits = math::flag_bit<uint8, 1>();
inline constexpr auto shr_sampled_pos_bits  = math::flag_bit<uint8, 2>();
inline constexpr auto shr_sampled_dir_bits  = math::flag_bit<uint8, 3>();

}// end namespace detail

enum class ESurfaceHitReason : detail::SurfaceHitReasonIntType
{
	/*! Invalid state. Most likely the reason has not been set. */
	Invalid = 0,

	/*! An uncategorized, unknown reason. */
	Unknown = detail::shr_unknown_bits,

	/*! A ray has hit the surface. */
	IncidentRay = detail::shr_incident_ray_bits,

	/*! A position from the surface has been picked. */
	SampledPos = detail::shr_sampled_pos_bits,

	/*! A direction from the surface has been picked. */
	SampledDir = detail::shr_sampled_dir_bits,

	/*! Both a position and a direction from the surface are picked. */
	SampledPosDir = detail::shr_sampled_pos_bits | detail::shr_sampled_dir_bits,
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(ESurfaceHitReason);

using SurfaceHitReason = TEnumFlags<ESurfaceHitReason>;

class SurfaceHit final
{
public:
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
	@return The ray of a hit event.
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
	HitProbe(probe).calcFullHitDetail(ray, &m_detail);
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
	return HitProbe(m_recordedProbe).reintersect(ray, probe, getRay());
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
