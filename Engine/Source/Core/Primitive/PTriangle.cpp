#include "Core/Primitive/PTriangle.h"
#include "Core/Primitive/PrimitiveMetadata.h"
#include "Math/Transform.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Entity/BoundingVolume/AABB.h"

#include <limits>

#define TRIANGLE_EPSILON 0.0001f

namespace ph
{

PTriangle::PTriangle(const PrimitiveMetadata* const metadata, const Vector3f& vA, const Vector3f& vB, const Vector3f& vC) :
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
}

PTriangle::~PTriangle() = default;

bool PTriangle::isIntersecting(const Ray& ray, Intersection* const out_intersection) const
{
	Vector3f localOrigin;
	Vector3f localDirection;
	m_metadata->m_worldToLocal->transformPoint(ray.getOrigin(), &localOrigin);
	m_metadata->m_worldToLocal->transformVector(ray.getDirection(), &localDirection);
	Ray localRay(localOrigin, localDirection.normalizeLocal());

	const float32 dist = localRay.getOrigin().sub(m_vA).dot(m_faceNormal) / (-localRay.getDirection().dot(m_faceNormal));

	// reject by distance
	if(dist < TRIANGLE_EPSILON || dist > std::numeric_limits<float32>::max() || dist != dist)
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
			hitPu = dist * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
			hitPv = dist * localRay.getDirection().z + localRay.getOrigin().z - m_vA.z;
			abPu = m_eAB.y;
			abPv = m_eAB.z;
			acPu = m_eAC.y;
			acPv = m_eAC.z;
		}
		// Z dominant, projection plane is XY
		else
		{
			hitPu = dist * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
			hitPv = dist * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
			abPu = m_eAB.x;
			abPv = m_eAB.y;
			acPu = m_eAC.x;
			acPv = m_eAC.y;
		}
	}
	// Y dominant, projection plane is ZX
	else if(abs(m_faceNormal.y) > abs(m_faceNormal.z))
	{
		hitPu = dist * localRay.getDirection().z + localRay.getOrigin().z - m_vA.z;
		hitPv = dist * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
		abPu = m_eAB.z;
		abPv = m_eAB.x;
		acPu = m_eAC.z;
		acPv = m_eAC.x;
	}
	// Z dominant, projection plane is XY
	else
	{
		hitPu = dist * localRay.getDirection().x + localRay.getOrigin().x - m_vA.x;
		hitPv = dist * localRay.getDirection().y + localRay.getOrigin().y - m_vA.y;
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

	Vector3f hitPosition;
	Vector3f hitNormal;
	Vector3f localHitNormal(m_nA.mul(1.0f - baryB - baryC).addLocal(m_nB.mul(baryB)).addLocal(m_nC.mul(baryC)));
	m_metadata->m_localToWorld->transformPoint(localRay.getDirection().mul(dist).addLocal(localRay.getOrigin()), &hitPosition);
	m_metadata->m_localToWorld->transformVector(localHitNormal, &hitNormal);
	//m_parentModel->getModelToWorldTransform()->transformVector(m_faceNormal, &hitNormal);

	Vector3f hitGeoNormal;
	m_metadata->m_localToWorld->transformVector(m_faceNormal, &hitGeoNormal);

	out_intersection->setHitPosition(hitPosition);
	out_intersection->setHitSmoothNormal(hitNormal.normalizeLocal());
	out_intersection->setHitGeoNormal(hitGeoNormal.normalizeLocal());
	out_intersection->setHitUVW(m_uvwA.mul(1.0f - baryB - baryC).addLocal(m_uvwB.mul(baryB)).addLocal(m_uvwC.mul(baryC)));
	out_intersection->setHitPrimitive(this);

	return true;
}

void PTriangle::calcAABB(AABB* const out_aabb) const
{
	Vector3f vA;
	Vector3f vB;
	Vector3f vC;
	m_metadata->m_localToWorld->transformPoint(m_vA, &vA);
	m_metadata->m_localToWorld->transformPoint(m_vB, &vB);
	m_metadata->m_localToWorld->transformPoint(m_vC, &vC);

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

	out_aabb->setMinVertex(Vector3f(minX - TRIANGLE_EPSILON, minY - TRIANGLE_EPSILON, minZ - TRIANGLE_EPSILON));
	out_aabb->setMaxVertex(Vector3f(maxX + TRIANGLE_EPSILON, maxY + TRIANGLE_EPSILON, maxZ + TRIANGLE_EPSILON));
}

bool PTriangle::isIntersecting(const AABB& aabb) const
{
	// Reference: Tomas Akenine-Moeller's "Fast 3D Triangle-Box Overlap Testing", which
	// is based on SAT but faster.

	// TODO: transform aabb to local space may be faster

	Vector3f tvA;
	Vector3f tvB;
	Vector3f tvC;
	m_metadata->m_localToWorld->transformPoint(m_vA, &tvA);
	m_metadata->m_localToWorld->transformPoint(m_vB, &tvB);
	m_metadata->m_localToWorld->transformPoint(m_vC, &tvC);

	// move the origin to the AABB's center
	const Vector3f aabbCenter(aabb.getMinVertex().add(aabb.getMaxVertex()).mulLocal(0.5f));
	tvA.subLocal(aabbCenter);
	tvB.subLocal(aabbCenter);
	tvC.subLocal(aabbCenter);

	Vector3f aabbHalfExtents = aabb.getMaxVertex().sub(aabbCenter);
	Vector3f projection;
	Vector3f sortedProjection;// (min, mid, max)

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

	Vector3f tNormal;
	m_metadata->m_localToWorld->transformVector(m_faceNormal, &tNormal);
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

}// end namespace ph