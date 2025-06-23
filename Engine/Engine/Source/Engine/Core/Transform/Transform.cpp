#include "Engine/Core/Transform/Transform.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/HitInfo.h"

#include <Common/assertion.h>

namespace ph
{

std::unique_ptr<Transform> Transform::genInversed() const
{
	return nullptr;
}

void Transform::transformV(const math::Vector3R& vector, const Time& time,
                           math::Vector3R* const out_vector) const
{
	transformVector(vector, time, out_vector);
}

void Transform::transformO(const math::Vector3R& orientation, const Time& time,
                           math::Vector3R* const out_orientation) const
{
	transformOrientation(orientation, time, out_orientation);
}

void Transform::transformP(const math::Vector3R& point, const Time& time,
                           math::Vector3R* const out_point) const
{
	transformPoint(point, time, out_point);
}

void Transform::transformV(const math::Vector3R& vector, math::Vector3R* const out_vector) const
{
	transformVector(vector, Time(), out_vector);
}

void Transform::transformO(const math::Vector3R& orientation, math::Vector3R* const out_orientation) const
{
	transformOrientation(orientation, Time(), out_orientation);
}

void Transform::transformP(const math::Vector3R& point, math::Vector3R* const out_point) const
{
	transformPoint(point, Time(), out_point);
}

void Transform::transform(const Ray& ray, Ray* const out_ray) const
{
	math::TLineSegment<real> transformedSegment;
	transformLineSegment(ray.getSegment(),
	                     ray.getTime(),
	                     &transformedSegment);

	out_ray->setSegment(transformedSegment);
}

void Transform::transform(const HitInfo& info, const Time& time,
                          HitInfo* const out_info) const
{
	math::Vector3R tPosition;
	math::Vector3R tGeometryNormal;
	math::Vector3R tShadingNormal;
	transformPoint(info.getPos(), time, &tPosition);
	transformOrientation(info.getGeometryNormal(), time, &tGeometryNormal);
	transformOrientation(info.getShadingNormal(), time, &tShadingNormal);

	out_info->setAttributes(tPosition,
	                        tGeometryNormal.normalizeLocal(),
	                        tShadingNormal.normalizeLocal());

	math::Vector3R tdPdU;
	math::Vector3R tdPdV;
	math::Vector3R tdNdU;
	math::Vector3R tdNdV;
	transformVector(info.getdPdU(), time, &tdPdU);
	transformVector(info.getdPdV(), time, &tdPdV);
	transformVector(info.getdNdU(), time, &tdNdU);
	transformVector(info.getdNdV(), time, &tdNdV);

	out_info->setDerivatives(tdPdU, tdPdV, tdNdU, tdNdV);
}

void Transform::transform(const math::AABB3D& aabb, const Time& time,
                          math::AABB3D* const out_aabb) const
{
	PH_ASSERT(out_aabb);

	// FIXME: slow!
	auto vertices = aabb.getBoundVertices();
	for(auto& vertex : vertices)
	{
		// TODO: consider moving this check to transformPoint()
		if(vertex.isFinite())
		{
			math::Vector3R tVertex;
			transformPoint(vertex, time, &tVertex);
			vertex = tVertex;
		}
	}
	
	*out_aabb = math::AABB3D(vertices[0]);
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

void Transform::transform(const math::AABB3D& aabb, math::AABB3D* const out_aabb) const
{
	transform(aabb, Time(), out_aabb);
}

}// end namespace ph
