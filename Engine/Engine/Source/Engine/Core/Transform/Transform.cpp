#include "Engine/Core/Transform/Transform.h"
#include "Engine/Core/Quantity/Time.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/TVector3.h"
#include "Engine/Core/HitInfo.h"
#include "Engine/Core/HitDetail.h"
#include "Engine/Core/SurfaceHit.h"

#include <Common/assertion.h>

namespace ph
{

std::unique_ptr<Transform> Transform::genInversed() const
{
	return nullptr;
}

void Transform::transformV(
	const math::Vector3R& vector,
	const Time&           time,
	math::Vector3R* const out_vector) const
{
	doTransformVector(vector, time, out_vector);
}

void Transform::transformO(
	const math::Vector3R& orientation,
	const Time&           time,
	math::Vector3R* const out_orientation) const
{
	doTransformOrientation(orientation, time, out_orientation);
}

void Transform::transformP(
	const math::Vector3R& point,
	const Time&           time,
	math::Vector3R* const out_point) const
{
	doTransformPoint(point, time, out_point);
}

void Transform::transformV(
	const math::Vector3R& vector,
	math::Vector3R* const out_vector) const
{
	doTransformVector(vector, Time{}, out_vector);
}

void Transform::transformO(
	const math::Vector3R& orientation,
	math::Vector3R* const out_orientation) const
{
	doTransformOrientation(orientation, Time{}, out_orientation);
}

void Transform::transformP(
	const math::Vector3R& point,
	math::Vector3R* const out_point) const
{
	doTransformPoint(point, Time{}, out_point);
}

void Transform::doTransformRay(
	const Ray& ray,
	Ray* const out_ray) const
{
	PH_ASSERT(out_ray);
	*out_ray = ray;

	math::TLineSegment<real> tSegment;
	doTransformLineSegment(ray.getSegment(),
	                       ray.getTime(),
	                       &tSegment);

	out_ray->setSegment(tSegment);
}

void Transform::doTransformHitInfo(
	const HitInfo& info,
	const Time&    time,
	HitInfo* const out_info) const
{
	PH_ASSERT(out_info);
	*out_info = info;

	math::Vector3R tPosition;
	math::Vector3R tGeometryNormal;
	doTransformPoint(info.getPos(), time, &tPosition);
	doTransformOrientation(info.getGeometryNormal(), time, &tGeometryNormal);

	if(!info.hasShadingNormal())
	{
		out_info->setAttributes(tPosition,
		                        tGeometryNormal.normalizeLocal());
	}
	else
	{
		math::Vector3R tShadingNormal;
		doTransformOrientation(info.getShadingNormal(), time, &tShadingNormal);

		if(!info.hasShadingTangent())
		{
			out_info->setAttributes(tPosition,
			                        tGeometryNormal.normalizeLocal(),
			                        tShadingNormal.normalizeLocal());
		}
		else
		{
			math::Vector3R tShadingTangent;
			doTransformVector(info.getShadingTangent(), time, &tShadingTangent);

			out_info->setAttributes(tPosition,
			                        tGeometryNormal.normalizeLocal(),
			                        tShadingNormal.normalizeLocal(),
			                        tShadingTangent.normalizeLocal());
		}
	}

	math::Vector3R tdPdU;
	math::Vector3R tdPdV;
	math::Vector3R tdNdU;
	math::Vector3R tdNdV;
	doTransformVector(info.getdPdU(), time, &tdPdU);
	doTransformVector(info.getdPdV(), time, &tdPdV);
	doTransformVector(info.getdNdU(), time, &tdNdU);
	doTransformVector(info.getdNdV(), time, &tdNdV);

	out_info->setDerivatives(tdPdU, tdPdV, tdNdU, tdNdV);
}

void Transform::transform(
	const HitDetail& detail,
	const Time&      time,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);
	*out_detail = detail;

	// We can only modify world hit info by transform
	transform(
		detail.getHitInfo(ECoordSys::World),
		time,
		&(out_detail->hitInfo(ECoordSys::World)));
}

void Transform::transform(
	const math::AABB3D& aabb,
	const Time&         time,
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
			doTransformPoint(vertex, time, &tVertex);
			vertex = tVertex;
		}
	}
	
	*out_aabb = math::AABB3D(vertices[0]);
	for(std::size_t i = 1; i < vertices.size(); i++)
	{
		out_aabb->unionWith(vertices[i]);
	}
}

void Transform::transform(
	const HitInfo& info,
	HitInfo* const out_info) const
{
	transform(info, Time{}, out_info);
}

void Transform::transform(
	const HitDetail& detail,
	HitDetail* const out_detail) const
{
	transform(detail, Time{}, out_detail);
}

void Transform::transform(
	const SurfaceHit& surfaceHit,
	SurfaceHit* const out_surfaceHit) const
{
	HitDetail tDetail;
	transform(surfaceHit.getDetail(), surfaceHit.getTime(), &tDetail);

	if(surfaceHit.hasFullHitDetail())
	{
		// We can only modify world hit info by transform
		tDetail.computeBasesOf(ECoordSys::World);
	}

	PH_ASSERT(out_surfaceHit);
	*out_surfaceHit = SurfaceHit(
		surfaceHit.getRay(),
		surfaceHit.getProbe(),
		tDetail,
		&SurfaceHit::getPrimitiveMetadataRef(tDetail),
		surfaceHit.getReason(),
		surfaceHit.hasFullHitDetail());
}

void Transform::transform(
	const math::AABB3D& aabb,
	math::AABB3D* const out_aabb) const
{
	transform(aabb, Time{}, out_aabb);
}

}// end namespace ph
