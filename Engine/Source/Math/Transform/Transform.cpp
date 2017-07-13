#include "Math/Transform/Transform.h"
#include "Core/Quantity/Time.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Core/Intersection.h"
#include "Core/BoundingVolume/AABB.h"

namespace ph
{

Transform::~Transform() = default;

void Transform::transformV(const Vector3R& vector, Vector3R* const out_vector) const
{
	transformVector(vector, Time(), out_vector);
}

void Transform::transformO(const Vector3R& orientation,
                           Vector3R* const out_orientation) const
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
	                     Time(),
	                     &(out_ray->getOrigin()), &(out_ray->getDirection()),
	                     &rayMinT, &rayMaxT);
	out_ray->setMinT(rayMinT);
	out_ray->setMaxT(rayMaxT);
}

void Transform::transform(const Intersection& intersection, Intersection* out_intersection) const
{
	Vector3R tHitPosition;
	Vector3R tHitSmoothNormal;
	Vector3R tHitGeoNormal;
	transformP(intersection.getHitPosition(),     &tHitPosition);
	transformO(intersection.getHitSmoothNormal(), &tHitSmoothNormal);
	transformO(intersection.getHitGeoNormal(),    &tHitGeoNormal);

	out_intersection->set(intersection.getHitPrimitive(),
	                      tHitPosition,
	                      tHitSmoothNormal.normalizeLocal(),
	                      tHitGeoNormal.normalizeLocal(),
	                      intersection.getHitUVW(),
	                      intersection.getHitRayT());
}

void Transform::transform(const AABB& aabb, AABB* out_aabb) const
{
	auto& vertices = aabb.getVertices();
	for(auto& vertex : vertices)
	{
		Vector3R tVertex;
		transformP(vertex, &tVertex);
		vertex = tVertex;
	}
	
	*out_aabb = AABB(vertices[0]);
	for(std::size_t i = 1; i < vertices.size(); i++)
	{
		out_aabb->unionWith(vertices[i]);
	}
}

}// end namespace ph