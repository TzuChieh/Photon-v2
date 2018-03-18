#pragma once

#include "Core/Intersectable/Primitive.h"
#include "Core/Intersectable/TransformedIntersectable.h"

namespace ph
{

class RigidTransform;

class TransformedPrimitive final : public Primitive
{
public:
	// A transformed primitive accepts only rigid transformations. This way, 
	// properties such as surface area and volume are guaranteed to be the same
	// during transformations.
	//
	TransformedPrimitive(
		const Primitive*      primitive,
		const RigidTransform* localToWorld,
		const RigidTransform* worldToLocal);

	virtual ~TransformedPrimitive() override;

	virtual inline bool isIntersecting(const Ray& ray) const override
	{
		return m_intersectable.isIntersecting(ray);
	}

	virtual inline bool isIntersecting(const Ray& ray, HitProbe& probe) const override
	{
		return m_intersectable.isIntersecting(ray, probe);
	}

	virtual inline void calcIntersectionDetail(
		const Ray&       ray,
		HitProbe&        probe,
		HitDetail* const out_detail) const override
	{
		m_intersectable.calcIntersectionDetail(ray, probe, out_detail);
	}

	virtual inline bool isIntersectingVolumeConservative(const AABB3D& aabb) const override
	{
		return m_intersectable.isIntersectingVolumeConservative(aabb);
	}

	virtual inline void calcAABB(AABB3D* const out_aabb) const override
	{
		m_intersectable.calcAABB(out_aabb);
	}

	virtual real calcPositionSamplePdfA(const Vector3R& position) const override;
	virtual void genPositionSample(PositionSample* out_sample) const override;

	virtual inline real calcExtendedArea() const override
	{
		return m_primitive->calcExtendedArea();
	}

private:
	const Primitive*         m_primitive;
	TransformedIntersectable m_intersectable;
	const RigidTransform*    m_localToWorld;
	const RigidTransform*    m_worldToLocal;
};

}// end namespace ph