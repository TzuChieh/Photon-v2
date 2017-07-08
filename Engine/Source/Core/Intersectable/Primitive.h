#pragma once

#include "Core/Intersectable/Intersectable.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"

namespace ph
{

class Ray;
class Intersection;
class AABB;
class PrimitiveMetadata;
class PositionSample;

class Primitive : public Intersectable
{
public:
	Primitive(const PrimitiveMetadata* const metadata);
	virtual ~Primitive() = 0;

	virtual bool isIntersecting(const Ray& ray, 
	                            Intersection* out_intersection) const = 0;
	virtual bool isIntersecting(const Ray& ray) const = 0;
	virtual bool isIntersectingVolumeConservative(const AABB& aabb) const = 0;
	virtual void calcAABB(AABB* out_aabb) const = 0;
	virtual real calcPositionSamplePdfA(const Vector3R& position) const = 0;
	virtual void genPositionSample(PositionSample* out_sample) const = 0;

	inline const PrimitiveMetadata* getMetadata() const
	{
		return m_metadata;
	}

	inline real getReciExtendedArea() const
	{
		return m_reciExtendedArea;
	}

	virtual real calcExtendedArea() const = 0;

protected:
	const PrimitiveMetadata* m_metadata;
	real m_reciExtendedArea;
};

}// end namespace ph