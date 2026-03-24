#pragma once

#include "Engine/Core/hit_fwd.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/math.h"
#include "Engine/Utility/TBitFlags.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph::lta { class SidednessAgreement; }

namespace ph
{

class Primitive;
class PrimitiveMetadata;
class SurfaceOptics;
class VolumeOptics;
class SurfaceEmitter;

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

/*! @brief General information about a ray-surface intersection event.
*/
class SurfaceHit final
{
public:
	static const Primitive& getPrimitiveRef(const HitDetail& detail);
	static const PrimitiveMetadata& getPrimitiveMetadataRef(const HitDetail& detail);

	/*! @brief Empty event. Nothing is hit.
	*/
	SurfaceHit();

	/*! @brief Construct from the ray and probe involved in a hit event.
	A full hit detail will be computed. If this is undesirable (e.g., full hit detail is not required),
	use the overload which let you set the hit detail directly.
	*/
	SurfaceHit(
		const Ray&               ray,
		const HitProbe&          probe,
		SurfaceHitReasons        reason);

	/*! @brief Contruct with all internal data specified explicitly.
	No extra calculation is performed.
	*/
	SurfaceHit(
		const Ray&               ray,
		const HitProbe&          probe,
		const HitDetail&         detail,
		const PrimitiveMetadata* metadata,
		SurfaceHitReasons        reason,
		bool                     hasFullHitDetail);

	SurfaceHit(
		const Ray&               ray,
		const HitProbe&          probe,
		ESurfaceHitReason        reason);

	SurfaceHit(
		const Ray&               ray,
		const HitProbe&          probe,
		const HitDetail&         detail,
		const PrimitiveMetadata* metadata,
		ESurfaceHitReason        reason,
		bool                     hasFullHitDetail);

	SurfaceHit switchChannel(uint32 newChannel) const;

	/*! @brief Intersect the intersected object again with a different ray.
	@param ray The different ray to use for intersection test.
	@param probe The probe to record the intersection.
	@note Generates hit event (with `ray` and `probe`).
	*/
	bool reintersect(const Ray& ray, HitProbe& probe) const;

	const HitDetail& getDetail() const;
	const HitProbe& getProbe() const;
	SurfaceHitReasons getReason() const;

	/*!
	This ray is the ray that caused the hit event and will not be affected when transforming
	a surface hit.
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
	bool hasFullHitDetail() const;

	/*!
	@return The primitive that is hit.
	*/
	const Primitive& getPrimitive() const;

	/*! @brief Helpers to retrieve information about the hit primitive.
	*/
	///@{
	const PrimitiveMetadata& getMetadata() const;
	const SurfaceEmitter& getSurfaceEmitter() const;
	const SurfaceOptics& getSurfaceOptics() const;
	const VolumeOptics* getInteriorOptics() const;
	const VolumeOptics* getExteriorOptics() const;
	///@}

private:
	Ray                      m_ray;
	HitProbe                 m_recordedProbe;
	HitDetail                m_detail;
	const PrimitiveMetadata* m_metadata;
	SurfaceHitReasons        m_reason;
	bool                     m_hasFullHitDetail;
};

// In-header Implementations:

inline SurfaceHit::SurfaceHit()
	: m_ray             {}
	, m_recordedProbe   {}
	, m_detail          {}
	, m_metadata        {}
	, m_reason          {}
	, m_hasFullHitDetail{false}
{}

inline SurfaceHit::SurfaceHit(
	const Ray&               ray,
	const HitProbe&          probe,
	SurfaceHitReasons        reason)

	: m_ray             {ray}
	, m_recordedProbe   {probe}
	, m_detail          {}
	, m_metadata        {nullptr}
	, m_reason          {reason}
	, m_hasFullHitDetail{false}
{
	probe.calcHitDetail(ray, &m_detail);

	// Cache this as obtaining metadata can incur some cost
	m_metadata = &getPrimitiveMetadataRef(m_detail);

	m_detail.computeBases();
	m_hasFullHitDetail = true;
}

inline SurfaceHit::SurfaceHit(
	const Ray&               ray, 
	const HitProbe&          probe, 
	const HitDetail&         detail,
	const PrimitiveMetadata* metadata,
	const SurfaceHitReasons  reason,
	const bool               hasFullHitDetail)

	: m_ray             {ray}
	, m_recordedProbe   {probe}
	, m_detail          {detail}
	, m_metadata        {metadata}
	, m_reason          {reason}
	, m_hasFullHitDetail{hasFullHitDetail}
{}

inline SurfaceHit::SurfaceHit(
	const Ray&              ray,
	const HitProbe&         probe,
	const ESurfaceHitReason reason)

	: SurfaceHit(ray, probe, SurfaceHitReasons(reason))
{}

inline SurfaceHit::SurfaceHit(
	const Ray&               ray,
	const HitProbe&          probe,
	const HitDetail&         detail,
	const PrimitiveMetadata* metadata,
	const ESurfaceHitReason  reason,
	const bool               hasFullHitDetail)

	: SurfaceHit(ray, probe, detail, metadata, SurfaceHitReasons(reason), hasFullHitDetail)
{}

inline bool SurfaceHit::reintersect(const Ray& ray, HitProbe& probe) const
{
	return m_recordedProbe.reintersect(ray, probe, getRay());
}

inline const HitDetail& SurfaceHit::getDetail() const
{
	return m_detail;
}

inline const HitProbe& SurfaceHit::getProbe() const
{
	return m_recordedProbe;
}

inline SurfaceHitReasons SurfaceHit::getReason() const
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

inline bool SurfaceHit::hasFullHitDetail() const
{
	return m_hasFullHitDetail;
}

inline const Primitive& SurfaceHit::getPrimitive() const
{
	return getPrimitiveRef(getDetail());
}

inline const Primitive& SurfaceHit::getPrimitiveRef(const HitDetail& detail)
{
	PH_ASSERT_MSG(detail.getPrimitive(),
		"Does not make sense to call the method if `SurfaceHit` hits nothing. "
		"You may miss a call to check for valid hit.");

	return *detail.getPrimitive();
}

}// end namespace ph
