#include "Core/Intersectable/PTriangle.h"
#include "Core/Intersectable/PrimitiveMetadata.h"
#include "Math/Transform/StaticTransform.h"
#include "Core/Ray.h"
#include "Core/HitProbe.h"
#include "Core/HitDetail.h"
#include "Core/Bound/AABB3D.h"
#include "Math/Random.h"
#include "Core/Sample/PositionSample.h"
#include "Math/TVector2.h"
#include "Math/Math.h"

#include <limits>
#include <iostream>

#define TRIANGLE_EPSILON 0.0001f

namespace ph
{

PTriangle::PTriangle(const PrimitiveMetadata* const metadata, const Vector3R& vA, const Vector3R& vB, const Vector3R& vC) :
	Primitive(metadata),
	m_vA(vA), m_vB(vB), m_vC(vC), 
	m_uvwA(0, 0, 0), m_uvwB(1, 0, 0), m_uvwC(0, 1, 0)
{
	m_eAB = m_vB.sub(m_vA);
	m_eAC = m_vC.sub(m_vA);

	m_faceNormal = m_eAB.cross(m_eAC).normalizeLocal();

	m_nA = m_faceNormal;
	m_nB = m_faceNormal;
	m_nC = m_faceNormal;

	m_reciExtendedArea = 1.0f / calcExtendedArea();
}

PTriangle::~PTriangle() = default;

bool PTriangle::isIntersecting(const Ray& ray, HitProbe& probe) const
{
	Vector3R rayDir = ray.getDirection();
	Vector3R vAt = m_vA.sub(ray.getOrigin());
	Vector3R vBt = m_vB.sub(ray.getOrigin());
	Vector3R vCt = m_vC.sub(ray.getOrigin());

	// find dominant dimension of ray direction, then make it Z, 
	// the rest dimensions are arbitrarily assigned
	if(std::abs(rayDir.x) > std::abs(rayDir.y))
	{
		// X dominant
		if(std::abs(rayDir.x) > std::abs(rayDir.z))
		{
			rayDir.set(rayDir.y, rayDir.z, rayDir.x);
			vAt.set(vAt.y, vAt.z, vAt.x);
			vBt.set(vBt.y, vBt.z, vBt.x);
			vCt.set(vCt.y, vCt.z, vCt.x);
		}
		// Z dominant
		else
		{
			// left as-is
		}
	}
	else
	{
		// Y dominant
		if(std::abs(rayDir.y) > std::abs(rayDir.z))
		{
			rayDir.set(rayDir.z, rayDir.x, rayDir.y);
			vAt.set(vAt.z, vAt.x, vAt.y);
			vBt.set(vBt.z, vBt.x, vBt.y);
			vCt.set(vCt.z, vCt.x, vCt.y);
		}
		// Z dominant
		else
		{
			// left as-is
		}
	}

	const real reciRayDirZ = 1.0_r / rayDir.z;
	const real shearX = -rayDir.x * reciRayDirZ;
	const real shearY = -rayDir.y * reciRayDirZ;
	const real shearZ = reciRayDirZ;

	vAt.x += shearX * vAt.z;
	vAt.y += shearY * vAt.z;
	vBt.x += shearX * vBt.z;
	vBt.y += shearY * vBt.z;
	vCt.x += shearX * vCt.z;
	vCt.y += shearY * vCt.z;

	real funcEa = vBt.x * vCt.y - vBt.y * vCt.x;
	real funcEb = vCt.x * vAt.y - vCt.y * vAt.x;
	real funcEc = vAt.x * vBt.y - vAt.y * vBt.x;

	// possibly fallback to higher precision test for triangle edges
	//
	if constexpr(sizeof(real) < sizeof(float64))
	{
		if(funcEa == 0.0_r || funcEb == 0.0_r || funcEc == 0.0_r)
		{
			const float64 funcEa64 = static_cast<float64>(vBt.x) * static_cast<float64>(vCt.y) -
			                         static_cast<float64>(vBt.y) * static_cast<float64>(vCt.x);
			const float64 funcEb64 = static_cast<float64>(vCt.x) * static_cast<float64>(vAt.y) -
			                         static_cast<float64>(vCt.y) * static_cast<float64>(vAt.x);
			const float64 funcEc64 = static_cast<float64>(vAt.x) * static_cast<float64>(vBt.y) -
			                         static_cast<float64>(vAt.y) * static_cast<float64>(vBt.x);
			
			funcEa = static_cast<real>(funcEa64);
			funcEb = static_cast<real>(funcEb64);
			funcEc = static_cast<real>(funcEc64);
		}
	}

	if((funcEa < 0.0_r || funcEb < 0.0_r || funcEc < 0.0_r) && (funcEa > 0.0_r || funcEb > 0.0_r || funcEc > 0.0_r))
	{
		return false;
	}

	const real determinant = funcEa + funcEb + funcEc;

	if(determinant == 0.0_r)
	{
		return false;
	}

	vAt.z *= shearZ;
	vBt.z *= shearZ;
	vCt.z *= shearZ;

	const real hitTscaled = funcEa * vAt.z + funcEb * vBt.z + funcEc * vCt.z;

	if(determinant > 0.0_r)
	{
		if(hitTscaled < ray.getMinT() * determinant || hitTscaled > ray.getMaxT() * determinant)
		{
			return false;
		}
	}
	else
	{
		if(hitTscaled > ray.getMinT() * determinant || hitTscaled < ray.getMaxT() * determinant)
		{
			return false;
		}
	}

	// so the ray intersects the triangle

	const real reciDeterminant = 1.0_r / determinant;
	const real baryA = funcEa * reciDeterminant;
	const real baryB = funcEb * reciDeterminant;
	const real baryC = funcEc * reciDeterminant;
	const real hitT = hitTscaled * reciDeterminant;

	probe.pushBaseHit(this, hitT);
	probe.cacheReal3(0, Vector3R(baryA, baryB, baryC));

	return true;
}

void PTriangle::calcIntersectionDetail(const Ray& ray, HitProbe& probe,
                                       HitDetail* const out_detail) const
{
	const Vector3R& hitPosition = ray.getOrigin().add(ray.getDirection().mul(probe.getHitRayT()));
	Vector3R hitBaryABC;
	probe.getCachedReal3(0, &hitBaryABC);

	const Vector3R& hitShadingNormal = Vector3R::weightedSum(
		m_nA, hitBaryABC.x,
		m_nB, hitBaryABC.y,
		m_nC, hitBaryABC.z).normalizeLocal();

	const Vector3R& hitUVW = Vector3R::weightedSum(
		m_uvwA, hitBaryABC.x,
		m_uvwB, hitBaryABC.y,
		m_uvwC, hitBaryABC.z);

	out_detail->getHitInfo(ECoordSys::LOCAL).setAttributes(
		hitPosition, 
		m_faceNormal, 
		hitShadingNormal, 
		probe.getHitRayT());

	Vector3R dPdU(0.0_r), dPdV(0.0_r);
	Vector3R dNdU(0.0_r), dNdV(0.0_r);
	const Vector2R dUVab(m_uvwB.x - m_uvwA.x, m_uvwB.y - m_uvwA.y);
	const Vector2R dUVac(m_uvwC.x - m_uvwA.x, m_uvwC.y - m_uvwA.y);
	const real uvDet = dUVab.x * dUVac.y - dUVab.y * dUVac.x;
	if(uvDet != 0.0_r)
	{
		const real reciUvDet = 1.0_r / uvDet;

		dPdU = m_eAB.mul(dUVac.y).add(m_eAC.mul(-dUVab.y)).mulLocal(reciUvDet);
		dPdV = m_eAB.mul(-dUVac.x).add(m_eAC.mul(dUVab.x)).mulLocal(reciUvDet);

		const Vector3R& dNab = m_nB.sub(m_nA);
		const Vector3R& dNac = m_nC.sub(m_nA);
		dNdU = dNab.mul(dUVac.y).add(dNac.mul(-dUVab.y)).mulLocal(reciUvDet);
		dNdV = dNab.mul(-dUVac.x).add(dNac.mul(dUVab.x)).mulLocal(reciUvDet);
	}
	
	out_detail->getHitInfo(ECoordSys::LOCAL).setDerivatives(
		dPdU, dPdV, dNdU, dNdV);

	out_detail->getHitInfo(ECoordSys::WORLD) = out_detail->getHitInfo(ECoordSys::LOCAL);
	out_detail->setMisc(this, hitUVW);
}

void PTriangle::calcAABB(AABB3D* const out_aabb) const
{
	real minX = m_vA.x, maxX = m_vA.x,
	     minY = m_vA.y, maxY = m_vA.y,
	     minZ = m_vA.z, maxZ = m_vA.z;

	if     (m_vB.x > maxX) maxX = m_vB.x;
	else if(m_vB.x < minX) minX = m_vB.x;
	if     (m_vB.y > maxY) maxY = m_vB.y;
	else if(m_vB.y < minY) minY = m_vB.y;
	if     (m_vB.z > maxZ) maxZ = m_vB.z;
	else if(m_vB.z < minZ) minZ = m_vB.z;

	if     (m_vC.x > maxX) maxX = m_vC.x;
	else if(m_vC.x < minX) minX = m_vC.x;
	if     (m_vC.y > maxY) maxY = m_vC.y;
	else if(m_vC.y < minY) minY = m_vC.y;
	if     (m_vC.z > maxZ) maxZ = m_vC.z;
	else if(m_vC.z < minZ) minZ = m_vC.z;

	out_aabb->setMinVertex(Vector3R(minX - TRIANGLE_EPSILON, minY - TRIANGLE_EPSILON, minZ - TRIANGLE_EPSILON));
	out_aabb->setMaxVertex(Vector3R(maxX + TRIANGLE_EPSILON, maxY + TRIANGLE_EPSILON, maxZ + TRIANGLE_EPSILON));
}


// Reference: Tomas Akenine-Moeller's "Fast 3D Triangle-Box Overlap Testing", 
// which is based on SAT but faster.
//
bool PTriangle::isIntersectingVolumeConservative(const AABB3D& volume) const
{
	Vector3R tvA = m_vA;
	Vector3R tvB = m_vB;
	Vector3R tvC = m_vC;

	// move the origin to the volume/AABB's center
	const Vector3R aabbCenter(volume.getMinVertex().add(volume.getMaxVertex()).mulLocal(0.5_r));
	tvA.subLocal(aabbCenter);
	tvB.subLocal(aabbCenter);
	tvC.subLocal(aabbCenter);

	Vector3R aabbHalfExtents = volume.getMaxVertex().sub(aabbCenter);
	Vector3R projection;
	Vector3R sortedProjection;// (min, mid, max)

	// test AABB face normals (x-, y- and z-axes)
	projection.set(tvA.x, tvB.x, tvC.x);
	projection.sort(&sortedProjection);
	if(sortedProjection.z < -aabbHalfExtents.x || sortedProjection.x > aabbHalfExtents.x)
		return false;

	projection.set(tvA.y, tvB.y, tvC.y);
	projection.sort(&sortedProjection);
	if(sortedProjection.z < -aabbHalfExtents.y || sortedProjection.x > aabbHalfExtents.y)
		return false;

	projection.set(tvA.z, tvB.z, tvC.z);
	projection.sort(&sortedProjection);
	if(sortedProjection.z < -aabbHalfExtents.z || sortedProjection.x > aabbHalfExtents.z)
		return false;

	// test triangle's face normal
	real trigOffset = Vector3R(tvA).dot(m_faceNormal);
	sortedProjection.z = std::abs(aabbHalfExtents.x * m_faceNormal.x)
	                   + std::abs(aabbHalfExtents.y * m_faceNormal.y)
	                   + std::abs(aabbHalfExtents.z * m_faceNormal.z);
	sortedProjection.x = -sortedProjection.z;
	if(sortedProjection.z < trigOffset || sortedProjection.x > trigOffset)
		return false;

	// test 9 edge cross-products (saves in projection)
	real aabbR;
	real trigE;// projected coordinate of a triangle's edge
	real trigV;// the remaining vertex's projected coordinate

	// TODO: precompute triangle edges

	// (1, 0, 0) cross (edge AB)
	projection.set(0.0_r, tvA.z - tvB.z, tvB.y - tvA.y);
	aabbR = aabbHalfExtents.y * std::abs(projection.y) + aabbHalfExtents.z * std::abs(projection.z);
	trigE = projection.y*tvA.y + projection.z*tvA.z;
	trigV = projection.y*tvC.y + projection.z*tvC.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 1, 0) cross (edge AB)
	projection.set(tvB.z - tvA.z, 0.0_r, tvA.x - tvB.x);
	aabbR = aabbHalfExtents.x * std::abs(projection.x) + aabbHalfExtents.z * std::abs(projection.z);
	trigE = projection.x*tvA.x + projection.z*tvA.z;
	trigV = projection.x*tvC.x + projection.z*tvC.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 0, 1) cross (edge AB)
	projection.set(tvA.y - tvB.y, tvB.x - tvA.x, 0.0_r);
	aabbR = aabbHalfExtents.x * std::abs(projection.x) + aabbHalfExtents.y * std::abs(projection.y);
	trigE = projection.x*tvA.x + projection.y*tvA.y;
	trigV = projection.x*tvC.x + projection.y*tvC.y;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (1, 0, 0) cross (edge BC)
	projection.set(0.0_r, tvB.z - tvC.z, tvC.y - tvB.y);
	aabbR = aabbHalfExtents.y * std::abs(projection.y) + aabbHalfExtents.z * std::abs(projection.z);
	trigE = projection.y*tvB.y + projection.z*tvB.z;
	trigV = projection.y*tvA.y + projection.z*tvA.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 1, 0) cross (edge BC)
	projection.set(tvC.z - tvB.z, 0.0_r, tvB.x - tvC.x);
	aabbR = aabbHalfExtents.x * std::abs(projection.x) + aabbHalfExtents.z * std::abs(projection.z);
	trigE = projection.x*tvB.x + projection.z*tvB.z;
	trigV = projection.x*tvA.x + projection.z*tvA.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 0, 1) cross (edge BC)
	projection.set(tvB.y - tvC.y, tvC.x - tvB.x, 0.0_r);
	aabbR = aabbHalfExtents.x * std::abs(projection.x) + aabbHalfExtents.y * std::abs(projection.y);
	trigE = projection.x*tvB.x + projection.y*tvB.y;
	trigV = projection.x*tvA.x + projection.y*tvA.y;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (1, 0, 0) cross (edge CA)
	projection.set(0.0_r, tvC.z - tvA.z, tvA.y - tvC.y);
	aabbR = aabbHalfExtents.y * std::abs(projection.y) + aabbHalfExtents.z * std::abs(projection.z);
	trigE = projection.y*tvC.y + projection.z*tvC.z;
	trigV = projection.y*tvB.y + projection.z*tvB.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 1, 0) cross (edge CA)
	projection.set(tvA.z - tvC.z, 0.0_r, tvC.x - tvA.x);
	aabbR = aabbHalfExtents.x * std::abs(projection.x) + aabbHalfExtents.z * std::abs(projection.z);
	trigE = projection.x*tvC.x + projection.z*tvC.z;
	trigV = projection.x*tvB.x + projection.z*tvB.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 0, 1) cross (edge CA)
	projection.set(tvC.y - tvA.y, tvA.x - tvC.x, 0.0_r);
	aabbR = aabbHalfExtents.x * std::abs(projection.x) + aabbHalfExtents.y * std::abs(projection.y);
	trigE = projection.x*tvC.x + projection.y*tvC.y;
	trigV = projection.x*tvB.x + projection.y*tvB.y;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// no separating axis found
	return true;
}

void PTriangle::genPositionSample(PositionSample* const out_sample) const
{
	const real A = std::sqrt(Random::genUniformReal_i0_e1());
	const real B = Random::genUniformReal_i0_e1();

	const Vector3R localPos = Vector3R::weightedSum(m_vA, 1.0_r - A, m_vB, A * (1.0_r - B), m_vC, B * A);
	Vector3R worldPos;
	//m_metadata->localToWorld.transformP(localPos, &worldPos);
	//out_sample->position = worldPos;
	out_sample->position = localPos;

	const Vector3R abc = calcBarycentricCoord(localPos);
	out_sample->uvw = m_uvwA.mul(1.0_r - abc.y - abc.z).addLocal(m_uvwB.mul(abc.y)).addLocal(m_uvwC.mul(abc.z));

	const Vector3R localNormal(m_nA.mul(1.0_r - abc.y - abc.z).addLocal(m_nB.mul(abc.y)).addLocal(m_nC.mul(abc.z)));
	Vector3R worldN;
	//m_metadata->localToWorld.transformVector(m_faceNormal, &worldN);

	//m_metadata->localToWorld.transformO(localNormal, &worldN);
	//out_sample->normal = worldN.normalizeLocal();
	out_sample->normal = localNormal.normalize();

	out_sample->pdf = m_reciExtendedArea;
}

real PTriangle::calcExtendedArea() const
{
	//Vector3R eAB;
	//Vector3R eAC;
	//m_metadata->localToWorld.transformV(m_eAB, &eAB);
	//m_metadata->localToWorld.transformV(m_eAC, &eAC);

	Vector3R eAB = m_eAB;
	Vector3R eAC = m_eAC;

	return eAB.cross(eAC).length() * 0.5_r;
}

Vector3R PTriangle::calcBarycentricCoord(const Vector3R& position) const
{
	// Reference: Real-Time Collision Detection, Volume 1, P.47 ~ P.48
	// Computes barycentric coordinates (a, b, c) for a position with 
	// respect to triangle ABC.

	const Vector3R eAP = position.sub(m_vA);

	const real d00 = m_eAB.dot(m_eAB);
	const real d01 = m_eAB.dot(m_eAC);
	const real d11 = m_eAC.dot(m_eAC);
	const real d20 = eAP.dot(m_eAB);
	const real d21 = eAP.dot(m_eAC);
	
	// TODO: check numeric stability

	const real reciDenom = 1.0_r / (d00 * d11 - d01 * d01);
	
	const real b = (d11 * d20 - d01 * d21) * reciDenom;
	const real c = (d00 * d21 - d01 * d20) * reciDenom;
	const real a = 1.0_r - b - c;

	return Vector3R(a, b, c);
}

real PTriangle::calcPositionSamplePdfA(const Vector3R& position) const
{
	// FIXME: primitive may have scale factor
	return m_reciExtendedArea;
}

}// end namespace ph