#pragma once

#include "Engine/Core/hit_fwd.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/math.h"
#include "Engine/Utility/TBitFlags.h"

#include <Common/assertion.h>
#include <Common/primitive_type.h>

namespace ph
{

class Primitive;
class PrimitiveMetadata;
class VolumeOptics;
class SurfaceHit;

enum class EVolumeHitReason : detail::HitReasonIntType
{
	/*! Invalid state. Most likely the reason has not been set. */
	Invalid = 0,

	/*! An uncategorized, unknown reason. */
	Unknown = detail::hr_unknown_bits,

	/*! A ray has hit the volume. */
	IncidentRay = detail::hr_incident_ray_bits,
};

PH_DEFINE_INLINE_ENUM_FLAG_OPERATORS(EVolumeHitReason);

using VolumeHitReason = TEnumFlags<EVolumeHitReason>;

/*! @brief General information about a ray-volume intersection event.
*/
class VolumeHit final
{
public:
	/*! @brief Empty event. Nothing is hit.
	Calling methods from an empty event is an error.
	*/
	VolumeHit();

	/*! @brief 
	*/
	VolumeHit(
		const SurfaceHit& X,
		const Ray& incidentRay,
		bool isInterior);

	VolumeHit(
		const Primitive* primitive,
		const Ray& ray,
		const math::Vector3R& pos,
		bool isInterior,
		VolumeHitReason reason);

	bool hasVolumeOptics() const;
	VolumeHitReason getReason() const;
	const Ray& getRay() const;

	/*! @brief Convenient method for `getRay()` where `getReason()` contains `ESurfaceHitReason::IncidentRay`. 
	*/
	const Ray& getIncidentRay() const;

	const Time& getTime() const;

	const Primitive& getPrimitive() const;
	const PrimitiveMetadata& getMetadata() const;

	/*!
	@return The position that is hit.
	*/
	math::Vector3R getPos() const;

	const VolumeOptics* getVolumeOptics() const;
	const VolumeOptics* getInteriorOptics() const;
	const VolumeOptics* getExteriorOptics() const;

private:
	const Primitive* m_primitive;
	math::Vector3R m_pos;
	Ray m_ray;
	bool m_isInterior;
	VolumeHitReason m_reason;
};

// In-header Implementations:

inline VolumeHit::VolumeHit()
#if PH_DEBUG
	: m_primitive{}
	, m_pos{}
	, m_ray{}
	, m_isInterior{}
	, m_reason{}
#endif
{}

inline VolumeHit::VolumeHit(
	const Primitive* primitive,
	const Ray& ray,
	const math::Vector3R& pos,
	bool isInterior,
	VolumeHitReason reason)

	: m_primitive{primitive}
	, m_pos{pos}
	, m_ray{ray}
	, m_isInterior{isInterior}
	, m_reason{reason}
{}

inline VolumeHitReason VolumeHit::getReason() const
{
	return m_reason;
}

inline const Ray& VolumeHit::getRay() const
{
	PH_ASSERT(!m_reason.hasExactly(EVolumeHitReason::Invalid));
	return m_ray;
}

inline const Ray& VolumeHit::getIncidentRay() const
{
	PH_ASSERT(m_reason.has(EVolumeHitReason::IncidentRay));
	return getRay();
}

inline const Time& VolumeHit::getTime() const
{
	return m_ray.getTime();
}

inline const Primitive& VolumeHit::getPrimitive() const
{
	PH_ASSERT_MSG(m_primitive,
		"Does not make sense to call this method if `VolumeHit` hits nothing; "
		"this should be handled by previous logics.");

	return *m_primitive;
}

inline math::Vector3R VolumeHit::getPos() const
{
	return m_pos;
}

inline const VolumeOptics* VolumeHit::getVolumeOptics() const
{
	return m_isInterior ? getInteriorOptics() : getExteriorOptics();
}

}// end namespace ph
