#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Common/assertion.h"

namespace ph
{

namespace math
{
	class RigidTransform;
}

class TransformedPrimitive : public Primitive
{
public:
	// A transformed primitive accepts only rigid transformations. This way, 
	// properties such as surface area and volume are guaranteed to be the same
	// during transformations.
	//
	TransformedPrimitive(
		const Primitive*            primitive,
		const math::RigidTransform* localToWorld,
		const math::RigidTransform* worldToLocal);

	inline bool isOccluding(const Ray& ray) const override
	{
		return m_intersectable.isOccluding(ray);
	}

	inline bool isIntersecting(const Ray& ray, HitProbe& probe) const override
	{
		if(m_intersectable.isIntersecting(ray, probe))
		{
			probe.pushIntermediateHit(this);
			return true;
		}
		else
		{
			return false;
		}
	}

	inline void calcIntersectionDetail(
		const Ray&       ray,
		HitProbe&        probe,
		HitDetail* const out_detail) const override
	{
		probe.popIntermediateHit();

		// If failed, it is likely to be caused by mismatched/missing probe push or pop in the hit stack
		PH_ASSERT(probe.getCurrentHit() == &m_intersectable);

		m_intersectable.calcIntersectionDetail(ray, probe, out_detail);
		out_detail->setHitIntrinsics(this, out_detail->getUVW(), out_detail->getRayT());
	}

	inline bool mayOverlapVolume(const math::AABB3D& aabb) const override
	{
		return m_intersectable.mayOverlapVolume(aabb);
	}

	inline math::AABB3D calcAABB() const override
	{
		return m_intersectable.calcAABB();
	}

	real calcPositionSamplePdfA(const math::Vector3R& position) const override;
	void genPositionSample(PrimitivePosSampleQuery& query, SampleFlow& sampleFlow) const override;

	bool uvwToPosition(
		const math::Vector3R& uvw,
		math::Vector3R*       out_position) const override;

	inline real calcExtendedArea() const override
	{
		// does not change under rigid transform
		return m_primitive->calcExtendedArea();
	}

	const PrimitiveMetadata* getMetadata() const override;

private:
	const Primitive*            m_primitive;
	TransformedIntersectable    m_intersectable;
	const math::RigidTransform* m_localToWorld;
	const math::RigidTransform* m_worldToLocal;
};

inline const PrimitiveMetadata* TransformedPrimitive::getMetadata() const
{
	return m_primitive->getMetadata();
}

}// end namespace ph
