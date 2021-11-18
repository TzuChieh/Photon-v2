#include "Core/Intersectable/PTriangle.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/Geometry/TAABB3D.h"
#include "Core/Sample/PositionSample.h"
#include "Math/TVector2.h"
#include "Math/math.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <limits>
#include <iostream>

namespace ph
{

PTriangle::PTriangle(const PrimitiveMetadata* const metadata, const math::Vector3R& vA, const math::Vector3R& vB, const math::Vector3R& vC) :

	Primitive(metadata),

	m_triangle(vA, vB, vC),
	m_uvwA(0, 0, 0), m_uvwB(1, 0, 0), m_uvwC(0, 1, 0)
{
	m_faceNormal = m_triangle.getFaceNormalSafe(math::Vector3R(0, 1, 0));
	PH_ASSERT(m_faceNormal.isFinite() && m_faceNormal.length() > 0.0_r);

	m_nA = m_faceNormal;
	m_nB = m_faceNormal;
	m_nC = m_faceNormal;
}

bool PTriangle::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	real hitT;
	math::Vector3R hitBaryABCs;
	if(!m_triangle.isIntersecting(ray.getSegment(), &hitT, &hitBaryABCs))
	{
		return false;
	}

	probe.pushBaseHit(this, hitT);
	probe.cache(hitBaryABCs);
	return true;
}

void PTriangle::calcIntersectionDetail(const Ray& ray, HitProbe& probe,
                                       HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	math::Vector3R hitBaryABC;
	probe.getCached(&hitBaryABC);

	PH_ASSERT_MSG(!hitBaryABC.isZero() && hitBaryABC.isFinite(), 
		hitBaryABC.toString());

	const math::Vector3R hitPosition = m_triangle.barycentricToSurface(hitBaryABC);

	const auto hitShadingNormal = math::Vector3R::weightedSum(
		m_nA, hitBaryABC.x(),
		m_nB, hitBaryABC.y(),
		m_nC, hitBaryABC.z()).normalizeLocal();

	PH_ASSERT_MSG(hitPosition.isFinite() && hitShadingNormal.isFinite(), "\n"
		"hit-position       = " + hitPosition.toString() + "\n"
		"hit-shading-normal = " + hitShadingNormal.toString() + "\n");

	// TODO: respect primitive channel
	// (if it's default channel, use vertex uvw; otherwise, use mapper)

	const auto hitUVW = math::Vector3R::weightedSum(
		m_uvwA, hitBaryABC.x(),
		m_uvwB, hitBaryABC.y(),
		m_uvwC, hitBaryABC.z());

	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(
		hitPosition, 
		m_faceNormal, 
		hitShadingNormal);

	math::Vector3R dPdU(0.0_r), dPdV(0.0_r);
	math::Vector3R dNdU(0.0_r), dNdV(0.0_r);
	const math::Vector2R dUVab(m_uvwB.x() - m_uvwA.x(), m_uvwB.y() - m_uvwA.y());
	const math::Vector2R dUVac(m_uvwC.x() - m_uvwA.x(), m_uvwC.y() - m_uvwA.y());
	const real uvDet = dUVab.x() * dUVac.y() - dUVab.y() * dUVac.x();
	if(uvDet != 0.0_r)
	{
		const auto [eAB, eAC] = m_triangle.getEdgeVectors();
		const real rcpUvDet   = 1.0_r / uvDet;

		dPdU = eAB.mul(dUVac.y()).add(eAC.mul(-dUVab.y())).mulLocal(rcpUvDet);
		dPdV = eAB.mul(-dUVac.x()).add(eAC.mul(dUVab.x())).mulLocal(rcpUvDet);

		const math::Vector3R& dNab = m_nB.sub(m_nA);
		const math::Vector3R& dNac = m_nC.sub(m_nA);
		dNdU = dNab.mul(dUVac.y()).add(dNac.mul(-dUVab.y())).mulLocal(rcpUvDet);
		dNdV = dNab.mul(-dUVac.x()).add(dNac.mul(dUVab.x())).mulLocal(rcpUvDet);
	}
	
	out_detail->getHitInfo(ECoordSys::LOCAL).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);

	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
	out_detail->setMisc(this, hitUVW, probe.getHitRayT());

	PH_ASSERT_MSG(dPdU.isFinite() && dPdV.isFinite() &&
	              dNdU.isFinite() && dNdV.isFinite(), "\n"
		"dPdU = " + dPdU.toString() + ", dPdV = " + dPdV.toString() + "\n"
		"dNdU = " + dNdU.toString() + ", dNdV = " + dNdV.toString() + "\n");
}

math::AABB3D PTriangle::calcAABB() const
{
	return m_triangle.getAABB();
}

// Reference: Tomas Akenine-Moeller's "Fast 3D Triangle-Box Overlap Testing", 
// which is based on SAT but faster.
//
bool PTriangle::isIntersectingVolumeConservative(const math::AABB3D& volume) const
{
	math::Vector3R tvA = m_triangle.getVa();
	math::Vector3R tvB = m_triangle.getVb();
	math::Vector3R tvC = m_triangle.getVc();

	// move the origin to the volume/AABB's center
	const math::Vector3R aabbCenter(volume.getMinVertex().add(volume.getMaxVertex()).mulLocal(0.5_r));
	tvA.subLocal(aabbCenter);
	tvB.subLocal(aabbCenter);
	tvC.subLocal(aabbCenter);

	math::Vector3R aabbHalfExtents = volume.getMaxVertex().sub(aabbCenter);
	math::Vector3R projection;
	math::Vector3R sortedProjection;// (min, mid, max)

	// test AABB face normals (x-, y- and z-axes)
	projection.set({tvA.x(), tvB.x(), tvC.x()});
	projection.sort(&sortedProjection);
	if(sortedProjection.z() < -aabbHalfExtents.x() || sortedProjection.x() > aabbHalfExtents.x())
		return false;

	projection.set({tvA.y(), tvB.y(), tvC.y()});
	projection.sort(&sortedProjection);
	if(sortedProjection.z() < -aabbHalfExtents.y() || sortedProjection.x() > aabbHalfExtents.y())
		return false;

	projection.set({tvA.z(), tvB.z(), tvC.z()});
	projection.sort(&sortedProjection);
	if(sortedProjection.z() < -aabbHalfExtents.z() || sortedProjection.x() > aabbHalfExtents.z())
		return false;

	// test triangle's face normal
	real trigOffset = math::Vector3R(tvA).dot(m_faceNormal);
	sortedProjection.z() = std::abs(aabbHalfExtents.x() * m_faceNormal.x())
	                     + std::abs(aabbHalfExtents.y() * m_faceNormal.y())
	                     + std::abs(aabbHalfExtents.z() * m_faceNormal.z());
	sortedProjection.x() = -sortedProjection.z();
	if(sortedProjection.z() < trigOffset || sortedProjection.x() > trigOffset)
		return false;

	// test 9 edge cross-products (saves in projection)
	real aabbR;
	real trigE;// projected coordinate of a triangle's edge
	real trigV;// the remaining vertex's projected coordinate

	// TODO: precompute triangle edges

	// (1, 0, 0) cross (edge AB)
	projection.set({0.0_r, tvA.z() - tvB.z(), tvB.y() - tvA.y()});
	aabbR = aabbHalfExtents.y() * std::abs(projection.y()) + aabbHalfExtents.z() * std::abs(projection.z());
	trigE = projection.y()*tvA.y() + projection.z()*tvA.z();
	trigV = projection.y()*tvC.y() + projection.z()*tvC.z();
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 1, 0) cross (edge AB)
	projection.set({tvB.z() - tvA.z(), 0.0_r, tvA.x() - tvB.x()});
	aabbR = aabbHalfExtents.x() * std::abs(projection.x()) + aabbHalfExtents.z() * std::abs(projection.z());
	trigE = projection.x()*tvA.x() + projection.z()*tvA.z();
	trigV = projection.x()*tvC.x() + projection.z()*tvC.z();
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 0, 1) cross (edge AB)
	projection.set({tvA.y() - tvB.y(), tvB.x() - tvA.x(), 0.0_r});
	aabbR = aabbHalfExtents.x() * std::abs(projection.x()) + aabbHalfExtents.y() * std::abs(projection.y());
	trigE = projection.x()*tvA.x() + projection.y()*tvA.y();
	trigV = projection.x()*tvC.x() + projection.y()*tvC.y();
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (1, 0, 0) cross (edge BC)
	projection.set({0.0_r, tvB.z() - tvC.z(), tvC.y() - tvB.y()});
	aabbR = aabbHalfExtents.y() * std::abs(projection.y()) + aabbHalfExtents.z() * std::abs(projection.z());
	trigE = projection.y()*tvB.y() + projection.z()*tvB.z();
	trigV = projection.y()*tvA.y() + projection.z()*tvA.z();
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 1, 0) cross (edge BC)
	projection.set({tvC.z() - tvB.z(), 0.0_r, tvB.x() - tvC.x()});
	aabbR = aabbHalfExtents.x() * std::abs(projection.x()) + aabbHalfExtents.z() * std::abs(projection.z());
	trigE = projection.x()*tvB.x() + projection.z()*tvB.z();
	trigV = projection.x()*tvA.x() + projection.z()*tvA.z();
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 0, 1) cross (edge BC)
	projection.set({tvB.y() - tvC.y(), tvC.x() - tvB.x(), 0.0_r});
	aabbR = aabbHalfExtents.x() * std::abs(projection.x()) + aabbHalfExtents.y() * std::abs(projection.y());
	trigE = projection.x()*tvB.x() + projection.y()*tvB.y();
	trigV = projection.x()*tvA.x() + projection.y()*tvA.y();
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (1, 0, 0) cross (edge CA)
	projection.set({0.0_r, tvC.z() - tvA.z(), tvA.y() - tvC.y()});
	aabbR = aabbHalfExtents.y() * std::abs(projection.y()) + aabbHalfExtents.z() * std::abs(projection.z());
	trigE = projection.y()*tvC.y() + projection.z()*tvC.z();
	trigV = projection.y()*tvB.y() + projection.z()*tvB.z();
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 1, 0) cross (edge CA)
	projection.set({tvA.z() - tvC.z(), 0.0_r, tvC.x() - tvA.x()});
	aabbR = aabbHalfExtents.x() * std::abs(projection.x()) + aabbHalfExtents.z() * std::abs(projection.z());
	trigE = projection.x()*tvC.x() + projection.z()*tvC.z();
	trigV = projection.x()*tvB.x() + projection.z()*tvB.z();
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 0, 1) cross (edge CA)
	projection.set({tvC.y() - tvA.y(), tvA.x() - tvC.x(), 0.0_r});
	aabbR = aabbHalfExtents.x() * std::abs(projection.x()) + aabbHalfExtents.y() * std::abs(projection.y());
	trigE = projection.x()*tvC.x() + projection.y()*tvC.y();
	trigV = projection.x()*tvB.x() + projection.y()*tvB.y();
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// no separating axis found
	return true;
}

void PTriangle::genPositionSample(SampleFlow& sampleFlow, PositionSample* const out_sample) const
{
	const math::Vector3R abc = m_triangle.sampleToBarycentricOsada(
		sampleFlow.flow2D());

	const math::Vector3R localPos = m_triangle.barycentricToSurface(abc);
	//Vector3R worldPos;
	//m_metadata->localToWorld.transformP(localPos, &worldPos);
	//out_sample->position = worldPos;
	out_sample->position = localPos;

	//const Vector3R abc = calcBarycentricCoord(localPos);
	out_sample->uvw = m_uvwA.mul(1.0_r - abc.y() - abc.z()).addLocal(m_uvwB.mul(abc.y())).addLocal(m_uvwC.mul(abc.z()));

	const math::Vector3R localNormal(m_nA.mul(1.0_r - abc.y() - abc.z()).addLocal(m_nB.mul(abc.y())).addLocal(m_nC.mul(abc.z())));
	//Vector3R worldN;
	//m_metadata->localToWorld.transformVector(m_faceNormal, &worldN);

	//m_metadata->localToWorld.transformO(localNormal, &worldN);
	//out_sample->normal = worldN.normalizeLocal();
	out_sample->normal = localNormal.normalize();

	out_sample->pdf = this->PTriangle::calcPositionSamplePdfA(out_sample->position);

	PH_ASSERT(out_sample->normal.isFinite() && out_sample->normal.length() > 0.9_r);
}

real PTriangle::calcExtendedArea() const
{
	return m_triangle.getArea();
}

math::Vector3R PTriangle::calcBarycentricCoord(const math::Vector3R& position) const
{
	return m_triangle.surfaceToBarycentric(position);
}

real PTriangle::calcPositionSamplePdfA(const math::Vector3R& position) const
{
	return 1.0_r / this->PTriangle::calcExtendedArea();
}

}// end namespace ph
