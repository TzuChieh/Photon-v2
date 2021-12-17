#include "Math/Transform/Transform.h"
#include "Core/Quantity/Time.h"
#include "Core/Ray.h"
#include "Math/TVector3.h"
#include "Core/HitInfo.h"
#include "Common/assertion.h"

namespace ph::math
{

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
	Vector3R rayOrigin, rayDirection;
	real rayMinT, rayMaxT;
	transformLineSegment(ray.getOrigin(), ray.getDirection(), 
	                     ray.getMinT(), ray.getMaxT(), 
	                     ray.getTime(),
	                     &rayOrigin, &rayDirection,
	                     &rayMinT, &rayMaxT);

	out_ray->setOrigin(rayOrigin);
	out_ray->setDirection(rayDirection);
	out_ray->setMinT(rayMinT);
	out_ray->setMaxT(rayMaxT);
}

void Transform::transform(const HitInfo& info, const Time& time,
                          HitInfo* const out_info) const
{
	Vector3R tPosition;
	Vector3R tGeometryNormal;
	Vector3R tShadingNormal;
	transformPoint(info.getPosition(), time, &tPosition);
	transformOrientation(info.getGeometryNormal(), time, &tGeometryNormal);
	transformOrientation(info.getShadingNormal(), time, &tShadingNormal);

	out_info->setAttributes(tPosition,
	                        tGeometryNormal.normalizeLocal(),
	                        tShadingNormal.normalizeLocal());

	Vector3R tdPdU;
	Vector3R tdPdV;
	Vector3R tdNdU;
	Vector3R tdNdV;
	transformVector(info.getdPdU(), time, &tdPdU);
	transformVector(info.getdPdV(), time, &tdPdV);
	transformVector(info.getdNdU(), time, &tdNdU);
	transformVector(info.getdNdV(), time, &tdNdV);

	out_info->setDerivatives(tdPdU, tdPdV, tdNdU, tdNdV);
}

void Transform::transform(const AABB3D& aabb, const Time& time,
                          AABB3D* const out_aabb) const
{
	PH_ASSERT(out_aabb);

	// FIXME: slow!
	auto vertices = aabb.getBoundVertices();
	for(auto& vertex : vertices)
	{
		// TODO: consider moving this check to transformPoint()
		if(vertex.isFinite())
		{
			Vector3R tVertex;
			transformPoint(vertex, time, &tVertex);
			vertex = tVertex;
		}
	}
	
	*out_aabb = AABB3D(vertices[0]);
	for(std::size_t i = 1; i < vertices.size(); i++)
	{
		out_aabb->unionWith(vertices[i]);
	}
}

void Transform::transform(const HitInfo& info,
                          HitInfo* const out_info) const
{
	transform(info, Time(), out_info);
}

void Transform::transform(const AABB3D& aabb, AABB3D* const out_aabb) const
{
	transform(aabb, Time(), out_aabb);
}

}// end namespace ph::math
