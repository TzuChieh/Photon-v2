#include "Math/Transform/Transform.h"
#include "Core/Quantity/Time.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Core/Intersection.h"
#include "Core/BoundingVolume/AABB.h"

namespace ph
{

Transform::~Transform() = default;

std::unique_ptr<Transform> Transform::genInversed() const
{
	return nullptr;
}

void Transform::transformV(const Vector3R& vector, const Time& time, 
                           Vector3R* const out_vector) const
{
	transformVector(vector, time, out_vector);
}

void Transform::transformO(const Vector3R& orientation, const Time& time, 
                           Vector3R* const out_orientation) const
{
	transformOrientation(orientation, time, out_orientation);
}

void Transform::transformP(const Vector3R& point, const Time& time, 
                           Vector3R* const out_point) const
{
	transformPoint(point, time, out_point);
}

void Transform::transformV(const Vector3R& vector, Vector3R* const out_vector) const
{
	transformVector(vector, Time(), out_vector);
}

void Transform::transformO(const Vector3R& orientation, Vector3R* const out_orientation) const
{
	transformOrientation(orientation, Time(), out_orientation);
}
void Transform::transformP(const Vector3R& point, Vector3R* const out_point) const
{
	transformPoint(point, Time(), out_point);
}

void Transform::transform(const Ray& ray, Ray* const out_ray) const
{
	real rayMinT, rayMaxT;
	transformLineSegment(ray.getOrigin(), ray.getDirection(), 
	                     ray.getMinT(), ray.getMaxT(), 
	                     ray.getTime(),
	                     &(out_ray->getOrigin()), &(out_ray->getDirection()),
	                     &rayMinT, &rayMaxT);
	out_ray->setMinT(rayMinT);
	out_ray->setMaxT(rayMaxT);
}

void Transform::transform(const Intersection& intersection, const Time& time, 
                          Intersection* const out_intersection) const
{
	Vector3R tHitPosition;
	Vector3R tHitSmoothNormal;
	Vector3R tHitGeoNormal;
	transformPoint      (intersection.getHitPosition(),     time, &tHitPosition);
	transformOrientation(intersection.getHitSmoothNormal(), time, &tHitSmoothNormal);
	transformOrientation(intersection.getHitGeoNormal(),    time, &tHitGeoNormal);

	out_intersection->set(intersection.getHitPrimitive(),
	                      tHitPosition,
	                      tHitSmoothNormal.normalizeLocal(),
	                      tHitGeoNormal.normalizeLocal(),
	                      intersection.getHitUVW(),
	                      intersection.getHitRayT());
}

void Transform::transform(const AABB& aabb, const Time& time, 
                          AABB* const out_aabb) const
{
	auto vertices = aabb.getVertices();
	for(auto& vertex : vertices)
	{
		Vector3R tVertex;
		transformPoint(vertex, time, &tVertex);
		vertex = tVertex;
	}
	
	*out_aabb = AABB(vertices[0]);
	for(std::size_t i = 1; i < vertices.size(); i++)
	{
		out_aabb->unionWith(vertices[i]);
	}
}

void Transform::transform(const Intersection& intersection, 
                          Intersection* const out_intersection) const
{
	transform(intersection, Time(), out_intersection);
}

void Transform::transform(const AABB& aabb, AABB* const out_aabb) const
{
	transform(aabb, Time(), out_aabb);
}

}// end namespace ph