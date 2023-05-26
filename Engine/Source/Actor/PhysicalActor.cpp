#include "Actor/PhysicalActor.h"

namespace ph
{

void PhysicalActor::translate(const real amountX, const real amountY, const real amountZ)
{
	m_localToWorld.translate(amountX, amountY, amountZ);
}

void PhysicalActor::translate(const math::Vector3R& amount)
{
	m_localToWorld.translate(amount);
}

void PhysicalActor::rotate(const math::Vector3R& axis, const real additionalDegrees)
{
	m_localToWorld.rotate(axis, additionalDegrees);
}

void PhysicalActor::rotate(const math::QuaternionR& additionalRotation)
{
	m_localToWorld.rotate(additionalRotation);
}

void PhysicalActor::scale(const real uniformAmount)
{
	m_localToWorld.scale(uniformAmount);
}

void PhysicalActor::scale(const real amountX, const real amountY, const real amountZ)
{
	m_localToWorld.scale(amountX, amountY, amountZ);
}

void PhysicalActor::scale(const math::Vector3R& amount)
{
	m_localToWorld.scale(amount);
}

void PhysicalActor::setBaseTransform(const math::TDecomposedTransform<real>& baseLocalToWorld)
{
	m_localToWorld.set(baseLocalToWorld);
}

}// end namespace ph
