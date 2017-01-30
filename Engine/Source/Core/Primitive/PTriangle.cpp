#include "Core/Primitive/PTriangle.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Math/Transform.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Core/BoundingVolume/AABB.h"
#include "Math/random_number.h"
#include "Core/Sample/PositionSample.h"

#include <limits>

#define TRIANGLE_EPSILON 0.0001f

namespace ph
{

PTriangle::PTriangle(const PrimitiveMetadata* const metadata, const Vector3R& vA, const Vector3R& vB, const Vector3R& vC) :
	Primitive(metadata),
	m_vA(vA), m_vB(vB), m_vC(vC), 
	m_uvwA(0, 0, 0), m_uvwB(0, 0, 0), m_uvwC(0, 0, 0)
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

bool PTriangle::isIntersecting(const Ray& ray, Intersection* const out_intersection) const
{
	Ray localRay;
	m_metadata->worldToLocal.transformRay(ray, &localRay);

	// hitT's unit is in world space (localRay's direction can have scale factor)
	const float32 hitT = localRay.getOrigin().sub(m_vA).dot(m_faceNormal) / (-localRay.getDirection().dot(m_faceNormal));

	// reject by distance (NaN-aware)
	if(!(ray.getMinT() < hitT && hitT < ray.getMaxT()))
		return false;

	// projected hit point
	float32 hitPu, hitPv;

	// projected side vector AB and AC
	float32 abPu, abPv, acPu, acPv;

	// find dominant axis
	if(abs(m_faceNormal.x) > abs(m_faceNormal.y))
	{
		// X dominant, projection plane is YZ
		if(abs(m_faceNormal.x) > abs(m_faceNormal.z))
		{
			hitPu = hitT * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
			hitPv = hitT * localRay.getDirection().z + localRay.getOrigin().z - m_vA.z;
			abPu = m_eAB.y;
			abPv = m_eAB.z;
			acPu = m_eAC.y;
			acPv = m_eAC.z;
		}
		// Z dominant, projection plane is XY
		else
		{
			hitPu = hitT * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
			hitPv = hitT * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
			abPu = m_eAB.x;
			abPv = m_eAB.y;
			acPu = m_eAC.x;
			acPv = m_eAC.y;
		}
	}
	// Y dominant, projection plane is ZX
	else if(abs(m_faceNormal.y) > abs(m_faceNormal.z))
	{
		hitPu = hitT * localRay.getDirection().z + localRay.getOrigin().z - m_vA.z;
		hitPv = hitT * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
		abPu = m_eAB.z;
		abPv = m_eAB.x;
		acPu = m_eAC.z;
		acPv = m_eAC.x;
	}
	// Z dominant, projection plane is XY
	else
	{
		hitPu = hitT * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
		hitPv = hitT * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
		abPu = m_eAB.x;
		abPv = m_eAB.y;
		acPu = m_eAC.x;
		acPv = m_eAC.y;
	}

	// TODO: check if these operations are possible of producing NaNs

	// barycentric coordinate of vertex B in the projected plane
	const float32 baryB = (hitPu*acPv - hitPv*acPu) / (abPu*acPv - abPv*acPu);
	if(baryB < 0.0f) return false;

	// barycentric coordinate of vertex C in the projected plane
	const float32 baryC = (hitPu*abPv - hitPv*abPu) / (acPu*abPv - abPu*acPv);
	if(baryC < 0.0f) return false;

	if(baryB + baryC > 1.0f) return false;

	// so the ray intersects the triangle (TODO: reuse calculated results!)

	Vector3R hitPosition;
	Vector3R hitNormal;
	Vector3R localHitNormal(m_nA.mul(1.0f - baryB - baryC).addLocal(m_nB.mul(baryB)).addLocal(m_nC.mul(baryC)));
	m_metadata->localToWorld.transformPoint(localRay.getDirection().mul(hitT).addLocal(localRay.getOrigin()), &hitPosition);
	m_metadata->localToWorld.transformVector(localHitNormal, &hitNormal);
	//m_parentModel->getModelToWorldTransform()->transformVector(m_faceNormal, &hitNormal);

	Vector3R hitGeoNormal;
	m_metadata->localToWorld.transformVector(m_faceNormal, &hitGeoNormal);

	out_intersection->setHitPosition(hitPosition);
	out_intersection->setHitSmoothNormal(hitNormal.normalizeLocal());
	out_intersection->setHitGeoNormal(hitGeoNormal.normalizeLocal());
	out_intersection->setHitUVW(m_uvwA.mul(1.0f - baryB - baryC).addLocal(m_uvwB.mul(baryB)).addLocal(m_uvwC.mul(baryC)));
	out_intersection->setHitPrimitive(this);

	return true;
}

bool PTriangle::isIntersecting(const Ray& ray) const
{
	Ray localRay;
	m_metadata->worldToLocal.transformRay(ray, &localRay);

	// hitT's unit is in world space (localRay's direction can have scale factor)
	const float32 hitT = localRay.getOrigin().sub(m_vA).dot(m_faceNormal) / (-localRay.getDirection().dot(m_faceNormal));

	// reject by distance (NaN-aware)
	if(!(ray.getMinT() < hitT && hitT < ray.getMaxT()))
		return false;

	// projected hit point
	float32 hitPu, hitPv;

	// projected side vector AB and AC
	float32 abPu, abPv, acPu, acPv;

	// find dominant axis
	if(abs(m_faceNormal.x) > abs(m_faceNormal.y))
	{
		// X dominant, projection plane is YZ
		if(abs(m_faceNormal.x) > abs(m_faceNormal.z))
		{
			hitPu = hitT * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
			hitPv = hitT * localRay.getDirection().z + localRay.getOrigin().z - m_vA.z;
			abPu = m_eAB.y;
			abPv = m_eAB.z;
			acPu = m_eAC.y;
			acPv = m_eAC.z;
		}
		// Z dominant, projection plane is XY
		else
		{
			hitPu = hitT * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
			hitPv = hitT * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
			abPu = m_eAB.x;
			abPv = m_eAB.y;
			acPu = m_eAC.x;
			acPv = m_eAC.y;
		}
	}
	// Y dominant, projection plane is ZX
	else if(abs(m_faceNormal.y) > abs(m_faceNormal.z))
	{
		hitPu = hitT * localRay.getDirection().z + localRay.getOrigin().z - m_vA.z;
		hitPv = hitT * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
		abPu = m_eAB.z;
		abPv = m_eAB.x;
		acPu = m_eAC.z;
		acPv = m_eAC.x;
	}
	// Z dominant, projection plane is XY
	else
	{
		hitPu = hitT * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
		hitPv = hitT * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
		abPu = m_eAB.x;
		abPv = m_eAB.y;
		acPu = m_eAC.x;
		acPv = m_eAC.y;
	}

	// TODO: check if these operations are possible of producing NaNs

	// barycentric coordinate of vertex B in the projected plane
	const float32 baryB = (hitPu*acPv - hitPv*acPu) / (abPu*acPv - abPv*acPu);
	if(baryB < 0.0f) return false;

	// barycentric coordinate of vertex C in the projected plane
	const float32 baryC = (hitPu*abPv - hitPv*abPu) / (acPu*abPv - abPu*acPv);
	if(baryC < 0.0f) return false;

	if(baryB + baryC > 1.0f) return false;

	// so the ray intersects the triangle (TODO: reuse calculated results!)
	return true;
}

void PTriangle::calcAABB(AABB* const out_aabb) const
{
	Vector3R vA;
	Vector3R vB;
	Vector3R vC;
	m_metadata->localToWorld.transformPoint(m_vA, &vA);
	m_metadata->localToWorld.transformPoint(m_vB, &vB);
	m_metadata->localToWorld.transformPoint(m_vC, &vC);

	float32 minX = vA.x, maxX = vA.x,
		minY = vA.y, maxY = vA.y,
		minZ = vA.z, maxZ = vA.z;

	if(vB.x > maxX)      maxX = vB.x;
	else if(vB.x < minX) minX = vB.x;
	if(vB.y > maxY)      maxY = vB.y;
	else if(vB.y < minY) minY = vB.y;
	if(vB.z > maxZ)      maxZ = vB.z;
	else if(vB.z < minZ) minZ = vB.z;

	if(vC.x > maxX)      maxX = vC.x;
	else if(vC.x < minX) minX = vC.x;
	if(vC.y > maxY)      maxY = vC.y;
	else if(vC.y < minY) minY = vC.y;
	if(vC.z > maxZ)      maxZ = vC.z;
	else if(vC.z < minZ) minZ = vC.z;

	out_aabb->setMinVertex(Vector3R(minX - TRIANGLE_EPSILON, minY - TRIANGLE_EPSILON, minZ - TRIANGLE_EPSILON));
	out_aabb->setMaxVertex(Vector3R(maxX + TRIANGLE_EPSILON, maxY + TRIANGLE_EPSILON, maxZ + TRIANGLE_EPSILON));
}

bool PTriangle::isIntersectingVolume(const AABB& aabb) const
{
	// Reference: Tomas Akenine-Moeller's "Fast 3D Triangle-Box Overlap Testing", which
	// is based on SAT but faster.

	// TODO: transform aabb to local space may be faster

	Vector3R tvA;
	Vector3R tvB;
	Vector3R tvC;
	m_metadata->localToWorld.transformPoint(m_vA, &tvA);
	m_metadata->localToWorld.transformPoint(m_vB, &tvB);
	m_metadata->localToWorld.transformPoint(m_vC, &tvC);

	// move the origin to the AABB's center
	const Vector3R aabbCenter(aabb.getMinVertex().add(aabb.getMaxVertex()).mulLocal(0.5f));
	tvA.subLocal(aabbCenter);
	tvB.subLocal(aabbCenter);
	tvC.subLocal(aabbCenter);

	Vector3R aabbHalfExtents = aabb.getMaxVertex().sub(aabbCenter);
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

	Vector3R tNormal;
	m_metadata->localToWorld.transformVector(m_faceNormal, &tNormal);
	tNormal.normalizeLocal();

	// test triangle's face normal
	float32 trigOffset = tvA.dot(tNormal);
	sortedProjection.z = abs(aabbHalfExtents.x * tNormal.x)
	                   + abs(aabbHalfExtents.y * tNormal.y)
	                   + abs(aabbHalfExtents.z * tNormal.z);
	sortedProjection.x = -sortedProjection.z;
	if(sortedProjection.z < trigOffset || sortedProjection.x > trigOffset)
		return false;

	// test 9 edge cross-products (saves in projection)
	float32 aabbR;
	float32 trigE;// projected coordinate of a triangle's edge
	float32 trigV;// the remaining vertex's projected coordinate

					// TODO: precompute triangle edges

					// (1, 0, 0) cross (edge AB)
	projection.set(0.0f, tvA.z - tvB.z, tvB.y - tvA.y);
	aabbR = aabbHalfExtents.y * abs(projection.y) + aabbHalfExtents.z * abs(projection.z);
	trigE = projection.y*tvA.y + projection.z*tvA.z;
	trigV = projection.y*tvC.y + projection.z*tvC.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 1, 0) cross (edge AB)
	projection.set(tvB.z - tvA.z, 0.0f, tvA.x - tvB.x);
	aabbR = aabbHalfExtents.x * abs(projection.x) + aabbHalfExtents.z * abs(projection.z);
	trigE = projection.x*tvA.x + projection.z*tvA.z;
	trigV = projection.x*tvC.x + projection.z*tvC.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 0, 1) cross (edge AB)
	projection.set(tvA.y - tvB.y, tvB.x - tvA.x, 0.0f);
	aabbR = aabbHalfExtents.x * abs(projection.x) + aabbHalfExtents.y * abs(projection.y);
	trigE = projection.x*tvA.x + projection.y*tvA.y;
	trigV = projection.x*tvC.x + projection.y*tvC.y;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (1, 0, 0) cross (edge BC)
	projection.set(0.0f, tvB.z - tvC.z, tvC.y - tvB.y);
	aabbR = aabbHalfExtents.y * abs(projection.y) + aabbHalfExtents.z * abs(projection.z);
	trigE = projection.y*tvB.y + projection.z*tvB.z;
	trigV = projection.y*tvA.y + projection.z*tvA.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 1, 0) cross (edge BC)
	projection.set(tvC.z - tvB.z, 0.0f, tvB.x - tvC.x);
	aabbR = aabbHalfExtents.x * abs(projection.x) + aabbHalfExtents.z * abs(projection.z);
	trigE = projection.x*tvB.x + projection.z*tvB.z;
	trigV = projection.x*tvA.x + projection.z*tvA.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 0, 1) cross (edge BC)
	projection.set(tvB.y - tvC.y, tvC.x - tvB.x, 0.0f);
	aabbR = aabbHalfExtents.x * abs(projection.x) + aabbHalfExtents.y * abs(projection.y);
	trigE = projection.x*tvB.x + projection.y*tvB.y;
	trigV = projection.x*tvA.x + projection.y*tvA.y;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (1, 0, 0) cross (edge CA)
	projection.set(0.0f, tvC.z - tvA.z, tvA.y - tvC.y);
	aabbR = aabbHalfExtents.y * abs(projection.y) + aabbHalfExtents.z * abs(projection.z);
	trigE = projection.y*tvC.y + projection.z*tvC.z;
	trigV = projection.y*tvB.y + projection.z*tvB.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 1, 0) cross (edge CA)
	projection.set(tvA.z - tvC.z, 0.0f, tvC.x - tvA.x);
	aabbR = aabbHalfExtents.x * abs(projection.x) + aabbHalfExtents.z * abs(projection.z);
	trigE = projection.x*tvC.x + projection.z*tvC.z;
	trigV = projection.x*tvB.x + projection.z*tvB.z;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// (0, 0, 1) cross (edge CA)
	projection.set(tvC.y - tvA.y, tvA.x - tvC.x, 0.0f);
	aabbR = aabbHalfExtents.x * abs(projection.x) + aabbHalfExtents.y * abs(projection.y);
	trigE = projection.x*tvC.x + projection.y*tvC.y;
	trigV = projection.x*tvB.x + projection.y*tvB.y;
	if(trigE < trigV) { if(trigE > aabbR || trigV < -aabbR) return false; }
	else              { if(trigV > aabbR || trigE < -aabbR) return false; }

	// no separating axis found
	return true;
}

void PTriangle::genPositionSample(PositionSample* const out_sample) const
{
	const float32 A = std::sqrt(genRandomFloat32_0_1_uniform());
	const float32 B = genRandomFloat32_0_1_uniform();

	const Vector3R localPos = m_vA.mul(1.0f - A).addLocal(m_vB.mul(A * (1.0f - B))).addLocal(m_vC.mul(B * A));
	Vector3R worldPos;
	m_metadata->localToWorld.transformPoint(localPos, &worldPos);
	out_sample->position = worldPos;

	const Vector3R abc = calcBarycentricCoord(localPos);
	out_sample->uvw = m_uvwA.mul(1.0f - abc.y - abc.z).addLocal(m_uvwB.mul(abc.y)).addLocal(m_uvwC.mul(abc.z));

	const Vector3R localNormal(m_nA.mul(1.0f - abc.y - abc.z).addLocal(m_nB.mul(abc.y)).addLocal(m_nC.mul(abc.z)));
	Vector3R worldN;
	//m_metadata->localToWorld.transformVector(m_faceNormal, &worldN);
	m_metadata->localToWorld.transformVector(localNormal, &worldN);
	out_sample->normal = worldN.normalizeLocal();

	out_sample->pdf = m_reciExtendedArea;
}

float32 PTriangle::calcExtendedArea() const
{
	Vector3R eAB;
	Vector3R eAC;
	m_metadata->localToWorld.transformVector(m_eAB, &eAB);
	m_metadata->localToWorld.transformVector(m_eAC, &eAC);
	return eAB.cross(eAC).length() * 0.5f;
}

Vector3R PTriangle::calcBarycentricCoord(const Vector3R& position) const
{
	// Reference: Real-Time Collision Detection, Volume 1, P.47 ~ P.48
	// Computes barycentric coordinates (a, b, c) for a position with respect to triangle (A, B, C).

	const Vector3R eAP = position.sub(m_vA);

	const float32 d00 = m_eAB.dot(m_eAB);
	const float32 d01 = m_eAB.dot(m_eAC);
	const float32 d11 = m_eAC.dot(m_eAC);
	const float32 d20 = eAP.dot(m_eAB);
	const float32 d21 = eAP.dot(m_eAC);
	
	const float32 reciDenom = 1.0f / (d00 * d11 - d01 * d01);
	
	const float32 b = (d11 * d20 - d01 * d21) * reciDenom;
	const float32 c = (d00 * d21 - d01 * d20) * reciDenom;
	const float32 a = 1.0f - b - c;

	return Vector3R(a, b, c);
}

float32 PTriangle::calcPositionSamplePdfA(const Vector3R& position) const
{
	// FIXME: primitive may have scale factor
	return m_reciExtendedArea;
}

}// end namespace ph