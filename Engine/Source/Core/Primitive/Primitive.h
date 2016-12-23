#pragma once

namespace ph
{

class Ray;
class Intersection;
class AABB;
class PrimitiveMetadata;

class Primitive
{
public:
	Primitive(const PrimitiveMetadata* const metadata);
	virtual ~Primitive() = 0;

	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const = 0;
	virtual bool isIntersecting(const Ray& ray) const = 0;
	virtual bool isIntersectingVolume(const AABB& aabb) const = 0;
	virtual void calcAABB(AABB* const out_aabb) const = 0;

	inline const PrimitiveMetadata* getMetadata() const
	{
		return m_metadata;
	}

protected:
	const PrimitiveMetadata* m_metadata;
};

}// end namespace ph