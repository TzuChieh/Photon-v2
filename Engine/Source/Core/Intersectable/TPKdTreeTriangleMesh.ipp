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
		probe.cache(closestHitBary);
		probe.cache(closestHitFaceIndex);

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

	std::size_t    faceIndex;
	math::Vector3R bary;
	probe.getCached(faceIndex);
	probe.getCached(bary);

	PH_ASSERT_MSG(bary.isNotZero() && bary.isFinite(),
		bary.toString());

	const TriFace triFace = m_triMesh->getFace(faceIndex);
	const Triangle triangle(triFace.getVertices());

	const math::Vector3R position   = triangle.barycentricToSurface(bary);
	const math::Vector3R faceNormal = triangle.getFaceNormal();

	const auto shadingNormal = triFace.hasShadingNormal() ? 
		TriFace::interpolate(triFace.getShadingNormals(), bary) : faceNormal;

	PH_ASSERT_MSG(position.isFinite() && shadingNormal.isFinite(), "\n"
		"position       = " + position.toString() + "\n"
		"shading-normal = " + shadingNormal.toString() + "\n");

	// TODO: respect primitive channel
	// (if it's default channel, use vertex uvw; otherwise, use mapper)

	const auto uvw = triFace.hasTexCoords() ?
		TriFace::interpolate(triFace.getTexCoords(), bary) : math::Vector3R(0);

	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(
		position, 
		faceNormal, 
		shadingNormal);

	math::Vector3R dPdU(0.0_r), dPdV(0.0_r);
	math::Vector3R dNdU(0.0_r), dNdV(0.0_r);
	/*const math::Vector2R dUVab(m_uvwB.x - m_uvwA.x, m_uvwB.y - m_uvwA.y);
	const math::Vector2R dUVac(m_uvwC.x - m_uvwA.x, m_uvwC.y - m_uvwA.y);
	const real uvDet = dUVab.x * dUVac.y - dUVab.y * dUVac.x;
	if(uvDet != 0.0_r)
	{
		const auto [eAB, eAC] = m_triangle.getEdgeVectors();
		const real rcpUvDet   = 1.0_r / uvDet;

		dPdU = eAB.mul(dUVac.y).add(eAC.mul(-dUVab.y)).mulLocal(rcpUvDet);
		dPdV = eAB.mul(-dUVac.x).add(eAC.mul(dUVab.x)).mulLocal(rcpUvDet);

		const math::Vector3R& dNab = m_nB.sub(m_nA);
		const math::Vector3R& dNac = m_nC.sub(m_nA);
		dNdU = dNab.mul(dUVac.y).add(dNac.mul(-dUVab.y)).mulLocal(rcpUvDet);
		dNdV = dNab.mul(-dUVac.x).add(dNac.mul(dUVab.x)).mulLocal(rcpUvDet);
	}*/
	
	out_detail->getHitInfo(ECoordSys::LOCAL).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);

	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
	out_detail->setMisc(this, uvw, probe.getHitRayT());

	PH_ASSERT_MSG(dPdU.isFinite() && dPdV.isFinite() &&
	              dNdU.isFinite() && dNdV.isFinite(), "\n"
		"dPdU = " + dPdU.toString() + ", dPdV = " + dPdV.toString() + "\n"
		"dNdU = " + dNdU.toString() + ", dNdV = " + dNdV.toString() + "\n");
}

template<typename Index>
inline math::AABB3D TPKdTreeTriangleMesh<Index>::calcAABB() const
{
	return m_tree.getAABB();
}

}// end namespace ph
