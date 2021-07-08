#include "Actor/PhysicalActor.h"
#include "Math/math.h"
#include "Math/TVector3.h"

namespace ph
{

PhysicalActor::PhysicalActor() : 
	Actor(), 
	m_localToWorld()
{}

PhysicalActor::PhysicalActor(const PhysicalActor& other) : 
	Actor(other), 
	m_localToWorld()
{}

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

PhysicalActor& PhysicalActor::operator = (const PhysicalActor& rhs)
{
	Actor::operator = (rhs);

	m_localToWorld = rhs.m_localToWorld;

	return *this;
}

void swap(PhysicalActor& first, PhysicalActor& second)
{
	// Enable ADL
	using std::swap;

	// By swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<Actor&>(first), static_cast<Actor&>(second));
	swap(first.m_localToWorld,       second.m_localToWorld);
}

}// end namespace ph
