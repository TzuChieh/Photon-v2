#pragma once

#include "Engine/Core/hit_fwd.h"
#include "Engine/Core/HitEvent.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Ray.h"
#include "Engine/Core/Intersection/PrimitiveMetadata.h"
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

	/*! @brief Construct from a recorded hit event.
	A full hit detail will be computed. If this is undesirable (e.g., full hit detail is not
	required), use the overload that lets you set the hit detail directly.
	*/
	explicit SurfaceHit(const HitEvent& event);

	/*! @brief Construct with all internal data specified explicitly.
	No extra calculation is performed.
	*/
	SurfaceHit(
		const HitEvent&          event,
		const HitDetail&         detail,
		const PrimitiveMetadata* metadata,
		bool                     hasFullHitDetail);

	SurfaceHit switchChannel(uint32 newChannel) const;

	/*! @brief Intersect the intersected object again with a different ray.
	@param event The destination event that will record the new intersection, initialized with the different ray.
	*/
	bool reintersect(HitEvent& event) const;

	const HitDetail& getDetail() const;
	const HitEvent& getHitEvent() const;
	uint32 getChannel() const;
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
	HitEvent                 m_event;
	HitDetail                m_detail;
	const PrimitiveMetadata* m_metadata;
	bool                     m_hasFullHitDetail;
};

// In-header Implementations:

inline SurfaceHit::SurfaceHit()
	: m_event           {}
	, m_detail          {}
	, m_metadata        {}
	, m_hasFullHitDetail{false}
{}

inline SurfaceHit::SurfaceHit(const HitEvent& event)
	: m_event           {event}
	, m_detail          {}
	, m_metadata        {nullptr}
	, m_hasFullHitDetail{false}
{
	event.calcHitDetail(&m_detail);

	// Cache this as obtaining metadata can incur some cost
	m_metadata = &getPrimitiveMetadataRef(m_detail);

	m_detail.computeBases();
	m_hasFullHitDetail = true;
}

inline SurfaceHit::SurfaceHit(
	const HitEvent&          event,
	const HitDetail&         detail,
	const PrimitiveMetadata* metadata,
	const bool               hasFullHitDetail)

	: m_event           {event}
	, m_detail          {detail}
	, m_metadata        {metadata}
	, m_hasFullHitDetail{hasFullHitDetail}
{}

inline bool SurfaceHit::reintersect(HitEvent& event) const
{
	return m_event.reintersect(event);
}

inline const HitDetail& SurfaceHit::getDetail() const
{
	return m_detail;
}

inline const HitEvent& SurfaceHit::getHitEvent() const
{
	PH_ASSERT(!getReason().hasExactly(ESurfaceHitReason::Invalid));
	return m_event;
}

inline uint32 SurfaceHit::getChannel() const
{
	return m_event.getChannel();
}

inline SurfaceHitReasons SurfaceHit::getReason() const
{
	return m_event.getReason();
}

inline const Ray& SurfaceHit::getRay() const
{
	return getHitEvent().getRay();
}

inline const Ray& SurfaceHit::getIncidentRay() const
{
	PH_ASSERT(getReason().has(ESurfaceHitReason::IncidentRay));
	return getRay();
}

inline const Time& SurfaceHit::getTime() const
{
	return getRay().getTime();
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

inline const PrimitiveMetadata& SurfaceHit::getMetadata() const
{
	PH_ASSERT_MSG(m_metadata,
		"Does not make sense to call the method if `SurfaceHit` hits nothing. "
		"You may miss a call to check for valid hit.");

	return *m_metadata;
}

inline const SurfaceEmitter& SurfaceHit::getSurfaceEmitter() const
{
	return getMetadata().getSurface().getEmitter();
}

inline const SurfaceOptics& SurfaceHit::getSurfaceOptics() const
{
	return getMetadata().getSurface().getOptics();
}

inline const VolumeOptics* SurfaceHit::getInteriorOptics() const
{
	return getMetadata().getInterior().getOptics();
}

inline const VolumeOptics* SurfaceHit::getExteriorOptics() const
{
	return getMetadata().getExterior().getOptics();
}

}// end namespace ph
