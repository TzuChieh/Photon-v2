#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/TransformedIntersectable.h"
#include "Core/HitDetail.h"
#include "Core/HitProbe.h"
#include "Common/assertion.h"

#include <iostream>

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

		PH_ASSERT(probe.getCurrentHit() == &m_intersectable);
		m_intersectable.calcIntersectionDetail(ray, probe, out_detail);
		out_detail->setMisc(this, out_detail->getUvw(), out_detail->getRayT());
	}

	inline bool mayIntersectVolume(const math::AABB3D& aabb) const override
	{
		return m_intersectable.mayIntersectVolume(aabb);
	}

	inline math::AABB3D calcAABB() const override
	{
		return m_intersectable.calcAABB();
	}

	real calcPositionSamplePdfA(const math::Vector3R& position) const override;
	void genPositionSample(SampleFlow& sampleFlow, PositionSample* out_sample) const override;

	bool uvwToPosition(
		const math::Vector3R& uvw,
		math::Vector3R*       out_position) const override;

	inline real calcExtendedArea() const override
	{
		// does not change under rigid transform
		return m_primitive->calcExtendedArea();
	}

private:
	const Primitive*            m_primitive;
	TransformedIntersectable    m_intersectable;
	const math::RigidTransform* m_localToWorld;
	const math::RigidTransform* m_worldToLocal;
};

}// end namespace ph
