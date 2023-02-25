#include "Actor/PhysicalActor.h"
#include "Math/math.h"
#include "Math/TVector3.h"

namespace ph
{

void PhysicalActor::translate(const math::Vector3R& translation)
{
	m_localToWorld.translate(translation);
}

void PhysicalActor::rotate(const math::QuaternionR& rotation)
{
	m_localToWorld.rotate(rotation);
}

void PhysicalActor::scale(const math::Vector3R& scaleFactor)
{
	m_localToWorld.scale(scaleFactor);
}

void PhysicalActor::rotate(const math::Vector3R& axis, const real degrees)
{
	m_localToWorld.rotate(axis, degrees);
}

void PhysicalActor::translate(const real x, const real y, const real z)
{
	translate(math::Vector3R(x, y, z));
}

void PhysicalActor::scale(const real x, const real y, const real z)
{
	scale(math::Vector3R(x, y, z));
}

void PhysicalActor::scale(const real scaleFactor)
{
	scale(math::Vector3R(scaleFactor));
}

void PhysicalActor::setBaseTransform(const math::TDecomposedTransform<real>& baseLocalToWorld)
{
	m_localToWorld = baseLocalToWorld;
}

//const StaticTransform* PhysicalActor::getLocalToWorldTransform() const
//{
//	return &m_localToWorld;
//}
//
//const StaticTransform* PhysicalActor::getWorldToLocalTransform() const
//{
//	return &m_worldToLocal;
//}

//void PhysicalActor::updateTransforms(const StaticTransform& parentTransform, const StaticTransform& parentInverseTransform)
//{
//	m_localToWorld = m_transformInfo.genTransform(parentTransform);
//	m_worldToLocal = m_transformInfo.genInverseTransform(parentInverseTransform);
//}

}// end namespace ph
