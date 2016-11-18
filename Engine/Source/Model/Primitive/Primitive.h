#pragma once

namespace ph
{

class Ray;
class Intersection;
class Model;
class AABB;

class Primitive
{
public:
	Primitive(const Model* const parentModel);
	virtual ~Primitive() = 0;

	virtual bool isIntersecting(const Ray& ray, Intersection* const out_intersection) const = 0;
	virtual void calcAABB(AABB* const out_aabb) const = 0;

	inline const Model* getParentModel() const
	{
		return m_parentModel;
	}

private:
	const Model* const m_parentModel;
};

}// end namespace ph