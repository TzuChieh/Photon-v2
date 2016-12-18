#pragma once

namespace ph
{

class Ray;
class Intersection;
class AABB;
class Entity;

class Primitive
{
public:
	Primitive(const Entity* const parentEntity);
	virtual ~Primitive() = 0;

	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const = 0;
	virtual bool isIntersecting(const AABB& aabb) const = 0;
	virtual void calcAABB(AABB* const out_aabb) const = 0;

	inline const Entity* getParentEntity() const
	{
		return m_parentEntity;
	}

protected:
	const Entity* m_parentEntity;
};

}// end namespace ph