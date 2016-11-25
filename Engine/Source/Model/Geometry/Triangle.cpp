#include "Model/Geometry/Triangle.h"
#include "Model/Model.h"
#include "Core/Ray.h"
#include "Core/Intersection.h"
#include "Model/Primitive/BoundingVolume/AABB.h"

#include <limits>

#define TRIANGLE_EPSILON 0.0001f

namespace ph
{

Triangle::Triangle(const Model* const parentModel, const Vector3f& vA, const Vector3f& vB, const Vector3f& vC) :
	m_parentModel(parentModel), m_vA(vA), m_vB(vB), m_vC(vC)
{
	m_eAB = m_vB.sub(m_vA);
	m_eAC = m_vC.sub(m_vA);

	m_faceNormal = m_eAB.cross(m_eAC).normalizeLocal();
}

bool Triangle::isIntersecting(const Ray& ray, Intersection* const out_intersection) const
{
	Vector3f localOrigin;
	Vector3f localDirection;
	m_parentModel->getWorldToModelTransform()->transformPoint(ray.getOrigin(), &localOrigin);
	m_parentModel->getWorldToModelTransform()->transformVector(ray.getDirection(), &localDirection);
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
	m_parentModel->getModelToWorldTransform()->transformPoint(localRay.getDirection().mul(dist).addLocal(localRay.getOrigin()), &hitPosition);
	m_parentModel->getModelToWorldTransform()->transformVector(m_faceNormal, &hitNormal);

	out_intersection->setHitPosition(hitPosition);
	out_intersection->setHitNormal(hitNormal.normalizeLocal());
	out_intersection->setHitTriangle(this);

	return true;
}

void Triangle::calcAABB(AABB* const out_aabb) const
{
	Vector3f vA;
	Vector3f vB;
	Vector3f vC;
	m_parentModel->getModelToWorldTransform()->transformPoint(m_vA, &vA);
	m_parentModel->getModelToWorldTransform()->transformPoint(m_vB, &vB);
	m_parentModel->getModelToWorldTransform()->transformPoint(m_vC, &vC);

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

}// end namespace ph