#pragma once

#include "Engine/Core/Intersection/Primitive.h"
#include "Engine/Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Engine/Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Engine/Core/Intersection/primitive_decorations.h"
#include "Engine/Core/Transform/RigidTransform.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/Core/HitProbe.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/hash.h"

#include <Common/assertion.h>
#include <Common/compiler.h>

#include <utility>

namespace ph
{

/*! @brief Applies rigid transformation to a primitive.
A transformed primitive accepts only rigid transformations. This way,
properties such as surface area and volume are guaranteed to be the same
during transformations.
@tparam SHOULD_FLIP_NG Final cook-time decision to flip the world-space geometric normal.
*/
template<typename PrimitiveGetter, bool SHOULD_FLIP_NG = false>
class TTransformedPrimitive : public Primitive
{
	// FIXME: intersecting routines' time correctness
public:
	TTransformedPrimitive(
		PrimitiveGetter        primitiveGetter,
		const RigidTransform*  localToWorld,
		const RigidTransform*  worldToLocal);

	bool isOccluding(const Ray& ray) const override;

	bool isIntersecting(const Ray& ray, HitProbe& probe) const override;

	bool reintersect(
		const Ray& ray,
		HitProbe& probe,
		const Ray& srcRay,
		HitProbe& srcProbe) const override;

	void calcHitDetail(
		const Ray&       ray,
		HitProbe&        probe,
		HitDetail* const out_detail) const override;

	bool mayOverlapVolume(const math::AABB3D& volume) const override;
	math::AABB3D calcAABB() const override;

	void genPosSample(
		PrimitivePosSampleQuery& query,
		SampleFlow& sampleFlow,
		HitProbe& probe) const override;

	void calcPosPdf(PrimitivePosPdfQuery& query) const override;

	real calcExtendedArea() const override;

	uint32 numMetadataSlots() const override;

	uint32 toMetadataSlot(uint64 faceID) const override;

	const PrimitiveMetadata& getMetadata(uint32 slot) const override;

private:
	[[PH_NO_UNIQUE_ADDRESS]]
	PrimitiveGetter m_inner;

	const RigidTransform* m_localToWorld;
	const RigidTransform* m_worldToLocal;
};

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::TTransformedPrimitive(
	PrimitiveGetter       primitiveGetter,
	const RigidTransform* const localToWorld,
	const RigidTransform* const worldToLocal)

	: Primitive()
	
	, m_inner(std::move(primitiveGetter))
	, m_localToWorld(localToWorld)
	, m_worldToLocal(worldToLocal)
{
	PH_ASSERT(localToWorld);
	PH_ASSERT(worldToLocal);
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline bool TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::isOccluding(
	const Ray& ray) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	return m_inner().isOccluding(localRay);
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline bool TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::isIntersecting(
	const Ray& ray,
	HitProbe& probe) const
{
	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);
	if(m_inner().isIntersecting(localRay, probe))
	{
		probe.pushIntermediateHit(this);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline bool TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::reintersect(
	const Ray& ray,
	HitProbe& probe,
	const Ray& srcRay,
	HitProbe& srcProbe) const
{
	PH_ASSERT(srcProbe.getTopHit() == this);
	srcProbe.popHit();

	Ray localRay, localSrcRay;
	m_worldToLocal->transform(ray, &localRay);
	m_worldToLocal->transform(srcRay, &localSrcRay);
	if(srcProbe.getTopHit()->reintersect(localRay, probe, localSrcRay, srcProbe))
	{
		probe.pushIntermediateHit(this);
		return true;
	}
	else
	{
		return false;
	}
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline void TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::calcHitDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	// If failed, it is likely to be caused by: 1. mismatched/missing probe push or pop in
	// the hit stack; 2. the hit event is invalid
	PH_ASSERT(probe.getTopHit() == this);
	probe.popHit();

	Ray localRay;
	m_worldToLocal->transform(ray, &localRay);

	// Current hit is not necessary `m_inner()`. For example, if `m_inner()` contains
	// multiple instances then it could simply skip over to one of them.
	PH_ASSERT(probe.getTopHit());
	HitDetail localDetail;
	probe.getTopHit()->calcHitDetail(localRay, probe, &localDetail);

	*out_detail = localDetail;
	m_localToWorld->transform(
		localDetail.getHitInfo(ECoordSys::World), &(out_detail->hitInfo(ECoordSys::World)));

	const auto [meanFactor, maxFactor] = out_detail->getDistanceErrorFactors();
	out_detail->updateDistanceErrorFactors(meanFactor, maxFactor * 1.25_r);

	// This is a representative of the original primitive
	out_detail->updatePrimitive(this);

	out_detail->updateGlobalPrimitiveID(math::combine_hashes(
		out_detail->getGlobalPrimitiveID(),
		math::moremur_bit_mix_64(reinterpret_cast<uint64>(m_worldToLocal))));

	if constexpr(SHOULD_FLIP_NG)
	{
		out_detail->setFlippedGeometryNormal();
	}
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline bool TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::mayOverlapVolume(
	const math::AABB3D& volume) const
{
	return calcAABB().isIntersectingVolume(volume);
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline math::AABB3D TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::calcAABB() const
{
	const Time startTime(0, 0);
	const Time endTime(0, 1);
	const math::AABB3D localAABB = m_inner().calcAABB();

	math::AABB3D worldAABB;
	if(m_localToWorld->hasMotion(startTime, endTime))
	{
		m_localToWorld->calcSweepAABB(localAABB, startTime, endTime, &worldAABB);
	}
	else
	{
		m_localToWorld->transform(localAABB, startTime, &worldAABB);
	}

	return worldAABB;
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline void TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::genPosSample(
	PrimitivePosSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	PrimitivePosSampleQuery localQuery;
	if(query.inputs.getObservationPos())
	{
		math::Vector3R localObservationPos;
		m_worldToLocal->transformP(*query.inputs.getObservationPos(), &localObservationPos);

		localQuery.inputs.set(
			query.inputs.getTime(),
			localObservationPos,
			query.inputs.getUvw(),
			query.inputs.getUvwPdf(),
			query.inputs.suggestDir());
	}
	else
	{
		localQuery.inputs = query.inputs;
	}

	m_inner().genPosSample(localQuery, sampleFlow, probe);
	if(!localQuery.outputs)
	{
		return;
	}

	probe.pushIntermediateHit(this);

	// Make sure to forward all local query outputs to world query outputs

	math::Vector3R worldPos;
	m_localToWorld->transformP(localQuery.outputs.getPos(), &worldPos);
	query.outputs.setPos(worldPos);

	Ray worldRay;
	m_localToWorld->transform(localQuery.outputs.getObservationRay(), &worldRay);
	query.outputs.setObservationRay(worldRay);

	query.outputs.setPdfPos(localQuery.outputs.getPdfPos());
	query.outputs.setPdfDir(localQuery.outputs.getPdfDir());
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline void TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::calcPosPdf(
	PrimitivePosPdfQuery& query) const
{
	m_inner().calcPosPdf(query);
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline real TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::calcExtendedArea() const
{
	// Does not change under rigid transform
	return m_inner().calcExtendedArea();
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline uint32 TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::numMetadataSlots() const
{
	return m_inner().numMetadataSlots();
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline uint32 TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::toMetadataSlot(
	const uint64 faceID) const
{
	return m_inner().toMetadataSlot(faceID);
}

template<typename PrimitiveGetter, bool SHOULD_FLIP_NG>
inline const PrimitiveMetadata& TTransformedPrimitive<PrimitiveGetter, SHOULD_FLIP_NG>::getMetadata(
	const uint32 slot) const
{
	return m_inner().getMetadata(slot);
}

}// end namespace ph
