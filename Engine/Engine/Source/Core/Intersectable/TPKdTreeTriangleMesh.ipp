#pragma once

#include "Core/Intersectable/TPKdTreeTriangleMesh.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Math/TVector3.h"

#include <utility>
#include <limits>

namespace ph
{

template<typename Index>
inline TPKdTreeTriangleMesh<Index>::TPKdTreeTriangleMesh(
	const std::shared_ptr<IndexedTriMesh>& triMesh,
	math::IndexedKdtreeParams              params) : 

	m_triMesh(triMesh),
	m_tree(
		triMesh ? triMesh->numFaces() : 0,
		{triMesh},
		{},
		std::move(params))
{
	PH_ASSERT(triMesh);
}

template<typename Index>
inline bool TPKdTreeTriangleMesh<Index>::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	real           closestHitT = std::numeric_limits<real>::max();
	math::Vector3R closestHitBary;
	std::size_t    closestHitFaceIndex;

	const bool hasHit = m_tree.nearestTraversal(
		ray.getSegment(), 
		[ray, &closestHitT, &closestHitBary, &closestHitFaceIndex](
			const TriFace&                  triFace,
			const math::TLineSegment<real>& segment)
		-> std::optional<real>
		{
			const Triangle triangle(triFace.getVertices());
			const Ray      raySegment(segment, ray.getTime());

			real           hitT;
			math::Vector3R hitBary;
			if(triangle.isIntersecting(raySegment, &hitT, &hitBary))
			{
				if(hitT < closestHitT)
				{
					closestHitT         = hitT;
					closestHitBary      = hitBary;
					closestHitFaceIndex = triFace.getIndex();
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
		probe.pushCache(closestHitBary);
		probe.pushCache(closestHitFaceIndex);

		return true;
	}
	else
	{
		return false;
	}
}

template<typename Index>
inline void TPKdTreeTriangleMesh<Index>::calcIntersectionDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);
	PH_ASSERT(m_triMesh);

	const auto faceIndex = probe.popCache<std::size_t>();
	const auto bary = probe.popCache<math::Vector3R>();
	PH_ASSERT_MSG(bary.isNotZero() && bary.isFinite(),
		bary.toString());

	const TriFace  triFace = m_triMesh->getFace(faceIndex);
	const Triangle triangle(triFace.getVertices());

	const math::Vector3R position   = triangle.barycentricToSurface(bary);
	const math::Vector3R faceNormal = triangle.getFaceNormal();

	const auto shadingNormal = triFace.hasShadingNormal() ? 
		Triangle::interpolate(triFace.getShadingNormals(), bary) : faceNormal;

	const auto uvw = triFace.hasTexCoords() ?
		Triangle::interpolate(triFace.getTexCoords(), bary) : math::Vector3R(0);

	PH_ASSERT_MSG(position.isFinite() && shadingNormal.isFinite() && uvw.isFinite(), "\n"
		"position       = " + position.toString() + "\n"
		"shading-normal = " + shadingNormal.toString() + "\n"
		"uvw            = " + uvw.toString() + "\n");

	// TODO: respect primitive channel
	// (if it's default channel, use vertex uvw; otherwise, use mapper)

	math::Vector3R dPdU(0.0_r), dPdV(0.0_r);
	math::Vector3R dNdU(0.0_r), dNdV(0.0_r);
	if(triFace.hasTexCoords())
	{
		const std::array<math::Vector2R, 3> uvs = {
			{triFace.getTexCoords()[0].x, triFace.getTexCoords()[0].y},
			{triFace.getTexCoords()[1].x, triFace.getTexCoords()[1].y},
			{triFace.getTexCoords()[2].x, triFace.getTexCoords()[2].y}};

		if(!Triangle::calcSurfaceParamDerivatives(
			triFace.getVertices(), uvs,
			&dPdU, &dPdV))
		{
			dPdU.set(0); dPdV.set(0);
		}

		if(triFace.hasShadingNormals() && !Triangle::calcSurfaceParamDerivatives(
			triFace.getShadingNormals(), uvs,
			&dNdU, &dNdV))
		{
			dNdU.set(0); dNdV.set(0);
		}
	}

	PH_ASSERT_MSG(dPdU.isFinite() && dPdV.isFinite() &&
		dNdU.isFinite() && dNdV.isFinite(), "\n"
		"dPdU = " + dPdU.toString() + ", dPdV = " + dPdV.toString() + "\n"
		"dNdU = " + dNdU.toString() + ", dNdV = " + dNdV.toString() + "\n");
	
	out_detail->setHitIntrinsics(this, uvw, probe.getHitRayT());
	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(
		position,
		faceNormal,
		shadingNormal);
	out_detail->getHitInfo(ECoordSys::LOCAL).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);
	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
}

template<typename Index>
inline math::AABB3D TPKdTreeTriangleMesh<Index>::calcAABB() const
{
	return m_tree.getAABB();
}

}// end namespace ph
