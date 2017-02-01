#include "Actor/PhysicalActor.h"
#include "FileIO/InputPacket.h"
#include "Math/Math.h"
#include "Math/TVector3.h"

namespace ph
{

PhysicalActor::PhysicalActor() : 
	Actor(), 
	m_transformInfo(), m_localToWorld(), m_worldToLocal()
{

}

PhysicalActor::PhysicalActor(const PhysicalActor& other) : 
	Actor(other), 
	m_transformInfo(other.m_transformInfo), m_localToWorld(other.m_localToWorld), m_worldToLocal(other.m_worldToLocal)
{

}

PhysicalActor::PhysicalActor(const InputPacket& packet) : 
	Actor(packet),
	m_transformInfo(), m_localToWorld(), m_worldToLocal()
{

}

PhysicalActor::~PhysicalActor() = default;

void PhysicalActor::translate(const Vector3R& translation)
{
	m_transformInfo.setPosition(m_transformInfo.getPosition().add(translation));

	updateTransforms();
}

void PhysicalActor::rotate(const Vector3R& normalizedAxis, const real degrees)
{
	QuaternionR rotation(normalizedAxis, Math::toRadians(degrees));
	rotation.mulLocal(m_transformInfo.getRotation());
	rotation.normalizeLocal();
	m_transformInfo.setRotation(rotation);

	updateTransforms();
}

void PhysicalActor::scale(const Vector3R& scaleFactor)
{
	m_transformInfo.setScale(m_transformInfo.getScale().mul(scaleFactor));

	updateTransforms();
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

const Transform* PhysicalActor::getLocalToWorldTransform() const
{
	return &m_localToWorld;
}

const Transform* PhysicalActor::getWorldToLocalTransform() const
{
	return &m_worldToLocal;
}

void PhysicalActor::updateTransforms(const Transform& parentTransform, const Transform& parentInverseTransform)
{
	m_localToWorld = m_transformInfo.genTransform(parentTransform);
	m_worldToLocal = m_transformInfo.genInverseTransform(parentInverseTransform);
}

void swap(PhysicalActor& first, PhysicalActor& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<Actor&>(first), static_cast<Actor&>(second));
	swap(first.m_transformInfo,      second.m_transformInfo);
	swap(first.m_localToWorld,       second.m_localToWorld);
	swap(first.m_worldToLocal,       second.m_worldToLocal);
}

PhysicalActor& PhysicalActor::operator = (const PhysicalActor& rhs)
{
	m_transformInfo = rhs.m_transformInfo;
	m_localToWorld  = rhs.m_localToWorld;
	m_worldToLocal  = rhs.m_worldToLocal;

	return *this;
}

}// end namespace ph