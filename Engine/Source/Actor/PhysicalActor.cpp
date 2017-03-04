#include "Actor/PhysicalActor.h"
#include "FileIO/InputPacket.h"
#include "Math/Math.h"
#include "Math/TVector3.tpp"

namespace ph
{

PhysicalActor::PhysicalActor() : 
	Actor(), 
	m_localToWorld()
{

}

PhysicalActor::PhysicalActor(const PhysicalActor& other) : 
	Actor(other), 
	m_localToWorld()
{

}

PhysicalActor::PhysicalActor(const InputPacket& packet) : 
	Actor(packet), 
	m_localToWorld()
{

}

PhysicalActor::~PhysicalActor() = default;

void PhysicalActor::translate(const Vector3R& translation)
{
	m_localToWorld.translate(TVector3<hiReal>(translation));
}

void PhysicalActor::rotate(const QuaternionR& rotation)
{
	m_localToWorld.rotate(TQuaternion<hiReal>(rotation));
}

void PhysicalActor::scale(const Vector3R& scaleFactor)
{
	m_localToWorld.scale(TVector3<hiReal>(scaleFactor));
}

void PhysicalActor::rotate(const Vector3R& axis, const real degrees)
{
	m_localToWorld.rotate(TVector3<hiReal>(axis), static_cast<hiReal>(degrees));
}

void PhysicalActor::translate(const real x, const real y, const real z)
{
	translate(Vector3R(x, y, z));
}

void PhysicalActor::scale(const real x, const real y, const real z)
{
	scale(Vector3R(x, y, z));
}

void PhysicalActor::scale(const real scaleFactor)
{
	scale(Vector3R(scaleFactor));
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
	m_localToWorld = rhs.m_localToWorld;

	return *this;
}

void swap(PhysicalActor& first, PhysicalActor& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<Actor&>(first), static_cast<Actor&>(second));
	swap(first.m_localToWorld,        second.m_localToWorld);
}

}// end namespace ph