#include "Core/Intersection/PTriangle.h"
#include "Math/Transform/StaticAffineTransform.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Math/Geometry/TAABB3D.h"
#include "Core/Intersection/Query/PrimitivePosSampleQuery.h"
#include "Core/Intersection/Query/PrimitivePosPdfQuery.h"
#include "Math/TVector2.h"
#include "Math/math.h"
#include "Core/SampleGenerator/SampleFlow.h"

#include <limits>
#include <array>

namespace ph
{

PTriangle::PTriangle(const math::Vector3R& vA, const math::Vector3R& vB, const math::Vector3R& vC)

	: Primitive()

	, m_triangle(vA, vB, vC)
	, m_uvwA(0, 0, 0)
	, m_uvwB(1, 0, 0)
	, m_uvwC(0, 1, 0)
{
	m_faceNormal = m_triangle.safeGetFaceNormal(math::Vector3R(0, 1, 0));
	PH_ASSERT_MSG(m_faceNormal.isFinite(), m_faceNormal.toString());
	PH_ASSERT_IN_RANGE(m_faceNormal.lengthSquared(), 0.9_r, 1.1_r);

	m_nA = m_faceNormal;
	m_nB = m_faceNormal;
	m_nC = m_faceNormal;
}

bool PTriangle::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	real hitT;
	math::Vector3R hitBaryABC;
	if(!m_triangle.isIntersecting(ray.getSegment(), &hitT, &hitBaryABC))
	{
		return false;
	}

	probe.pushBaseHit(this, hitT);
	probe.pushCache(hitBaryABC);
	return true;
}

bool PTriangle::reintersect(
	const Ray& ray,
	HitProbe& probe,
	const Ray& /* srcRay */,
	HitProbe& srcProbe) const
{
	// Popping may seem redudant, but it is not. Upper-level intersectables may cache their
	// own data, and popping data consistently during probe consumption makes sure everybody
	// gets their data.
	srcProbe.popCache<math::Vector3R>();
	srcProbe.popHit();

	return PTriangle::isIntersecting(ray, probe);
}

void PTriangle::calcHitDetail(
	const Ray&       ray,
	HitProbe&        probe,
	HitDetail* const out_detail) const
{
	PH_ASSERT(out_detail);

	const auto hitBaryABC = probe.popCache<math::Vector3R>();
	PH_ASSERT_MSG(!hitBaryABC.isZero() && hitBaryABC.isFinite(), 
		hitBaryABC.toString());

	probe.popHit();

	const auto hitPosition = m_triangle.barycentricToSurface(hitBaryABC);
	const auto hitShadingNormal = Triangle::interpolate(m_nA, m_nB, m_nC, hitBaryABC).normalize();
	const auto hitUVW = Triangle::interpolate(m_uvwA, m_uvwB, m_uvwC, hitBaryABC);

	PH_ASSERT_MSG(hitPosition.isFinite() && hitShadingNormal.isFinite(), "\n"
		"hitPosition      = " + hitPosition.toString() + "\n"
		"hitShadingNormal = " + hitShadingNormal.toString() + "\n");
	PH_ASSERT_IN_RANGE(hitShadingNormal.lengthSquared(), 0.9_r, 1.1_r);

	// TODO: respect primitive channel
	// (if it's default channel, use vertex uvw; otherwise, use mapper)

	out_detail->getHitInfo(ECoordSys::Local).setAttributes(
		hitPosition, 
		m_faceNormal, 
		hitShadingNormal);

	math::Vector3R dPdU(0.0_r), dPdV(0.0_r);
	math::Vector3R dNdU(0.0_r), dNdV(0.0_r);
	{
		const std::array<math::Vector2R, 3> uvs = {
			math::Vector2R(m_uvwA.x(), m_uvwA.y()),
			math::Vector2R(m_uvwB.x(), m_uvwB.y()),
			math::Vector2R(m_uvwC.x(), m_uvwC.y())};

		if(!Triangle::calcSurfaceParamDerivatives(
			{m_triangle.getVa(), m_triangle.getVb(), m_triangle.getVc()}, uvs, &dPdU, &dPdV))
		{
			dPdU.set(0); dPdV.set(0);
		}

		if(!Triangle::calcSurfaceParamDerivatives(
			{m_nA, m_nB, m_nC}, uvs, &dNdU, &dNdV))
		{
			dNdU.set(0); dNdV.set(0);
		}
	}
	
	out_detail->getHitInfo(ECoordSys::Local).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);

	out_detail->getHitInfo(ECoordSys::World) = out_detail->getHitInfo(ECoordSys::Local);
	out_detail->setHitIntrinsics(
		this, 
		hitUVW, 
		probe.getHitRayT(), 
		HitDetail::NO_FACE_ID, 
		FaceTopology({EFaceTopology::Planar, EFaceTopology::Triangular}));

	constexpr auto meanFactor = 5e-8_r;
	out_detail->setDistanceErrorFactors(meanFactor, meanFactor * 5e2_r);

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
bool PTriangle::mayOverlapVolume(const math::AABB3D& volume) const
{
	math::Vector3R tvA = m_triangle.getVa();
	math::Vector3R tvB = m_triangle.getVb();
	math::Vector3R tvC = m_triangle.getVc();

	// Move the origin to the volume/AABB's center
	const math::Vector3R aabbCenter(volume.getMinVertex().add(volume.getMaxVertex()).mulLocal(0.5_r));
	tvA.subLocal(aabbCenter);
	tvB.subLocal(aabbCenter);
	tvC.subLocal(aabbCenter);

	math::Vector3R aabbHalfExtents = volume.getMaxVertex().sub(aabbCenter);
	math::Vector3R projection;
	math::Vector3R sortedProjection;// (min, mid, max)

	// Test AABB face normals (x-, y- and z-axes)
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

	// Test triangle's face normal
	real trigOffset = math::Vector3R(tvA).dot(m_faceNormal);
	sortedProjection.z() = std::abs(aabbHalfExtents.x() * m_faceNormal.x())
	                     + std::abs(aabbHalfExtents.y() * m_faceNormal.y())
	                     + std::abs(aabbHalfExtents.z() * m_faceNormal.z());
	sortedProjection.x() = -sortedProjection.z();
	if(sortedProjection.z() < trigOffset || sortedProjection.x() > trigOffset)
		return false;

	// Test 9 edge cross-products (saves in projection)
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

	// No separating axis found
	return true;
}

void PTriangle::genPosSample(
	PrimitivePosSampleQuery& query,
	SampleFlow& sampleFlow,
	HitProbe& probe) const
{
	real pdfA;
	const auto baryABC = m_triangle.sampleToBarycentricOsada(
		sampleFlow.flow2D(), &pdfA);

	const auto pos = m_triangle.barycentricToSurface(baryABC);

	const Ray observationRay(
		query.inputs.getObservationPos().value_or(pos),
		pos - query.inputs.getObservationPos().value_or(pos),
		0,
		1,
		query.inputs.getTime());

	query.outputs.setPos(pos);
	query.outputs.setPdfPos(lta::PDF::A(pdfA));
	query.outputs.setObservationRay(observationRay);

	probe.pushBaseHit(this, observationRay.getMaxT());
	probe.pushCache(baryABC);
}

void PTriangle::calcPosPdf(PrimitivePosPdfQuery& query) const
{
	query.outputs.setPdf(lta::PDF::A(m_triangle.uniformSurfaceSamplePdfA()));
}

real PTriangle::calcExtendedArea() const
{
	return m_triangle.getArea();
}

}// end namespace ph
