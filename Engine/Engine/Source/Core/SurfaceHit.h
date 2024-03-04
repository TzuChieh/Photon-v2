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
	Invalid       = 0,
	Unknown       = detail::shr_unknown_bits,
	IncidentRay   = detail::shr_incident_ray_bits,
	SampledPos    = detail::shr_sampled_pos_bits,
	SampledDir    = detail::shr_sampled_dir_bits,
	SampledPosDir = detail::shr_sampled_pos_bits | detail::shr_sampled_dir_bits
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(ESurfaceHitReason);

using SurfaceHitReason = TEnumFlags<ESurfaceHitReason>;

class SurfaceHit final
{
public:
	SurfaceHit();

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
	bool reintersect(const Ray& ray, HitProbe& probe) const;

	bool hasSurfaceOptics() const;
	bool hasInteriorOptics() const;
	bool hasExteriorOptics() const;

	const HitDetail& getDetail() const;
	SurfaceHitReason getReason() const;
	const Ray& getRay() const;
	const Ray& getIncidentRay() const;
	const Time& getTime() const;
	math::Vector3R getPosition() const;
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

inline math::Vector3R SurfaceHit::getPosition() const
{
	return m_detail.getPosition();
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
