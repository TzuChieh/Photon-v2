#pragma once

#include "Core/Intersection/TPIndexedKdTreeTriangleMesh.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/TVector3.h"

#include <Common/logging.h>

#include <utility>
#include <limits>

namespace ph
{

template<typename Index>
inline TPIndexedKdTreeTriangleMesh<Index>::TPIndexedKdTreeTriangleMesh(
	IndexedTriangleBuffer* const triangleBuffer,
	math::IndexedKdtreeParams params)

	: m_triangleBuffer(triangleBuffer)
	, m_kdTree(
		triangleBuffer ? triangleBuffer->numFaces() : 0,
		IndexToTriangle{triangleBuffer},
		TriangleToAABB{},
		std::move(params))
{
	PH_ASSERT(triangleBuffer);

	if(triangleBuffer && triangleBuffer->numFaces() > std::numeric_limits<Index>::max())
	{
		PH_DEFAULT_LOG(Error,
			"Kd-Tree index type max value is {} which can not store {} triangles, please use a "
			"wider index type",
			std::numeric_limits<Index>::max(), triangleBuffer->numFaces());
	}
}

template<typename Index>
inline bool TPIndexedKdTreeTriangleMesh<Index>::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	real closestHitT = std::numeric_limits<real>::max();
	ClosestHitProbeResult closestHit;

	const bool hasHit = m_kdTree.nearestTraversal(
		ray.getSegment(), 
		[ray, &closestHitT, &closestHit](
			const Triangle& triangle,
			const math::TLineSegment<real>& segment,
			const std::size_t triangleIndex)
		-> std::optional<real>
		{
			real hitT;
			math::Vector3R hitBary;
			if(triangle.isIntersecting(segment, &hitT, &hitBary))
			{
				if(hitT < closestHitT)
				{
					closestHitT = hitT;
					closestHit.bary = hitBary;
					closestHit.faceIndex = static_cast<Index>(triangleIndex);
				}

				return hitT;
			}
			else
			{
				return {};
			}
		});

	if(hasHit)
	{
		probe.pushBaseHit(this, closestHitT);
		probe.pushCache(closestHit);

		return true;
	}
	else
	{
		return false;
	}
}

template<typename Index>
inline bool TPIndexedKdTreeTriangleMesh<Index>::reintersect(
	const Ray& ray,
	HitProbe& probe,
	const Ray& /* srcRay */,
	HitProbe& srcProbe) const
{
	// Popping may seem redudant, but it is not. Upper-level intersectables may cache their
	// own data, and popping data consistently during probe consumption makes sure everybody
	// gets their data.
	srcProbe.popCache<ClosestHitProbeResult>();
	srcProbe.popHit();

	return TPIndexedKdTreeTriangleMesh::isIntersecting(ray, probe);
}

template<typename Index>
inline void TPIndexedKdTreeTriangleMesh<Index>::calcHitDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);
	PH_ASSERT(m_triangleBuffer);

	const auto closestHit = probe.popCache<ClosestHitProbeResult>();
	PH_ASSERT_MSG(!closestHit.bary.isZero() && closestHit.bary.isFinite(),
		closestHit.bary.toString());

	probe.popHit();

	const auto& positions = m_triangleBuffer->getPositions(closestHit.faceIndex);
	const auto& texCoords = m_triangleBuffer->getTexCoords(closestHit.faceIndex);
	const auto& normals = m_triangleBuffer->getNormals(closestHit.faceIndex);

	const Triangle triangle(positions);
	const math::Vector3R position = triangle.barycentricToSurface(closestHit.bary);
	const math::Vector3R faceNormal = triangle.getFaceNormal();

	const bool hasTexCoords = m_triangleBuffer->hasTexCoord();
	const bool hasShadingNormal = m_triangleBuffer->hasNormal();

	const auto shadingNormal = hasShadingNormal
		? Triangle::interpolate(normals, closestHit.bary).normalize()
		: faceNormal;

	const auto uvw = hasTexCoords
		? Triangle::interpolate(texCoords, closestHit.bary)
		: math::Vector3R(0);

	PH_ASSERT_MSG(position.isFinite() && shadingNormal.isFinite() && uvw.isFinite(), "\n"
		"position       = " + position.toString() + "\n"
		"shading-normal = " + shadingNormal.toString() + "\n"
		"uvw            = " + uvw.toString() + "\n");

	// TODO: respect primitive channel
	// (if it's default channel, use vertex uvw; otherwise, use mapper)

	math::Vector3R dPdU(0.0_r), dPdV(0.0_r);
	math::Vector3R dNdU(0.0_r), dNdV(0.0_r);
	if(hasTexCoords)
	{
		const std::array<math::Vector2R, 3> uvs = {
			math::Vector2R(texCoords[0].x(), texCoords[0].y()),
			math::Vector2R(texCoords[1].x(), texCoords[1].y()),
			math::Vector2R(texCoords[2].x(), texCoords[2].y())};

		if(!Triangle::calcSurfaceParamDerivatives(
			positions, uvs,
			&dPdU, &dPdV))
		{
			dPdU.set(0); dPdV.set(0);
		}

		if(hasShadingNormal && !Triangle::calcSurfaceParamDerivatives(
			normals, uvs,
			&dNdU, &dNdV))
		{
			dNdU.set(0); dNdV.set(0);
		}
	}

	PH_ASSERT_MSG(dPdU.isFinite() && dPdV.isFinite() &&
		dNdU.isFinite() && dNdV.isFinite(), "\n"
		"dPdU = " + dPdU.toString() + ", dPdV = " + dPdV.toString() + "\n"
		"dNdU = " + dNdU.toString() + ", dNdV = " + dNdV.toString() + "\n");
	
	out_detail->setHitIntrinsics(
		this, 
		uvw, 
		probe.getHitRayT(),
		HitDetail::NO_FACE_ID,// TODO
		FaceTopology({EFaceTopology::Planar, EFaceTopology::Triangular}));
	out_detail->getHitInfo(ECoordSys::Local).setAttributes(
		position,
		faceNormal,
		shadingNormal);
	out_detail->getHitInfo(ECoordSys::Local).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);
	out_detail->getHitInfo(ECoordSys::World) = out_detail->getHitInfo(ECoordSys::Local);

	constexpr auto meanFactor = 5e-8_r;
	out_detail->setDistanceErrorFactors(meanFactor, meanFactor * 5e2_r);
}

template<typename Index>
inline math::AABB3D TPIndexedKdTreeTriangleMesh<Index>::calcAABB() const
{
	return m_kdTree.getAABB();
}

}// end namespace ph
