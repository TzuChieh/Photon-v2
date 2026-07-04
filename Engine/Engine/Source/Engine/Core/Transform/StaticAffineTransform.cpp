#include "Engine/Core/Transform/StaticAffineTransform.h"
#include "Engine/Core/HitInfo.h"
#include "Engine/Core/Ray.h"
#include "Engine/Math/TVector3.h"

#include <Common/assertion.h>

namespace ph
{

const StaticAffineTransform& StaticAffineTransform::IDENTITY()
{
	static const StaticAffineTransform identityTransform(math::Matrix4R::makeIdentity(), math::Matrix4R::makeIdentity());
	return identityTransform;
}

StaticAffineTransform::StaticAffineTransform() :
	StaticAffineTransform(math::Matrix4R::makeIdentity(), math::Matrix4R::makeIdentity())
{}

StaticAffineTransform::StaticAffineTransform(const math::Matrix4R& transform, const math::Matrix4R& inverseTransform) :
	m_transformMatrix(transform), m_inverseTransformMatrix(inverseTransform)
{
	// TODO: assert on actually affine
}

std::unique_ptr<Transform> StaticAffineTransform::genInversed() const
{
	auto inversed = std::make_unique<StaticAffineTransform>();
	inversed->m_transformMatrix        = m_inverseTransformMatrix;
	inversed->m_inverseTransformMatrix = m_transformMatrix;

	return std::move(inversed);
}

void StaticAffineTransform::doTransformRay(
	const Ray& ray,
	Ray* const out_ray) const
{
	PH_ASSERT(out_ray);
	*out_ray = ray;

	math::Vector3R tOrigin;
	StaticAffineTransform::doTransformPoint(ray.getOrigin(), ray.getTime(), &tOrigin);

	math::Vector3R tDir;
	StaticAffineTransform::doTransformVector(ray.getDir(), ray.getTime(), &tDir);

	out_ray->setOrigin(tOrigin);
	out_ray->setDir(tDir);
}

void StaticAffineTransform::doTransformHitInfo(
	const HitInfo& info,
	const Time&    time,
	HitInfo* const out_info) const
{
	PH_ASSERT(out_info);
	*out_info = info;

	math::Vector3R tPosition;
	math::Vector3R tGeometryNormal;
	StaticAffineTransform::doTransformPoint(info.getPos(), time, &tPosition);
	StaticAffineTransform::doTransformOrientation(info.getGeometryNormal(), time, &tGeometryNormal);

	if(!info.hasShadingNormal())
	{
		out_info->setAttributes(tPosition,
		                        tGeometryNormal.normalizeLocal());
	}
	else
	{
		math::Vector3R tShadingNormal;
		StaticAffineTransform::doTransformOrientation(info.getShadingNormal(), time, &tShadingNormal);

		if(!info.hasShadingTangent())
		{
			out_info->setAttributes(tPosition,
			                        tGeometryNormal.normalizeLocal(),
			                        tShadingNormal.normalizeLocal());
		}
		else
		{
			math::Vector3R tShadingTangent;
			StaticAffineTransform::doTransformVector(info.getShadingTangent(), time, &tShadingTangent);

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
	StaticAffineTransform::doTransformVector(info.getdPdU(), time, &tdPdU);
	StaticAffineTransform::doTransformVector(info.getdPdV(), time, &tdPdV);
	StaticAffineTransform::doTransformVector(info.getdNdU(), time, &tdNdU);
	StaticAffineTransform::doTransformVector(info.getdNdV(), time, &tdNdV);

	out_info->setDerivatives(tdPdU, tdPdV, tdNdU, tdNdV);
}

void StaticAffineTransform::doTransformLineSegment(
	const math::TLineSegment<real>& segment,
	const Time&                     time,
	math::TLineSegment<real>* const out_segment) const
{
	math::Vector3R tOrigin;
	StaticAffineTransform::doTransformPoint(segment.getOrigin(), time, &tOrigin);

	math::Vector3R tDir;
	StaticAffineTransform::doTransformVector(segment.getDir(), time, &tDir);

	out_segment->setOrigin(tOrigin);
	out_segment->setDir(tDir);
	out_segment->setRange(segment.getMinT(), segment.getMaxT());
}

}// end namespace ph
