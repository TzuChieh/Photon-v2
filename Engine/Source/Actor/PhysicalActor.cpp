#include "Actor/PhysicalActor.h"
#include "FileIO/InputPacket.h"
#include "Math/Math.h"

namespace ph
{

PhysicalActor::PhysicalActor() : 
	Actor(), 
	m_entityTransformInfo(), m_localToWorld(), m_worldToLocal()
{

}

PhysicalActor::PhysicalActor(const PhysicalActor& other) : 
	Actor(other), 
	m_entityTransformInfo(other.m_entityTransformInfo), m_localToWorld(other.m_localToWorld), m_worldToLocal(other.m_worldToLocal)
{

}

PhysicalActor::PhysicalActor(const InputPacket& packet) : 
	Actor(packet),
	m_entityTransformInfo(), m_localToWorld(), m_worldToLocal()
{

}

PhysicalActor::~PhysicalActor() = default;

void PhysicalActor::translate(const Vector3f& translation)
{
	m_entityTransformInfo.setPosition(m_entityTransformInfo.getPosition().add(translation));

	updateTransforms();
}

void PhysicalActor::translate(const float32 x, const float32 y, const float32 z)
{
	translate(Vector3f(x, y, z));
}

void PhysicalActor::rotate(const Vector3f& normalizedAxis, const float32 degrees)
{
	Quaternion rotation(normalizedAxis, Math::toRadians(degrees));
	rotation.mulLocal(m_entityTransformInfo.getRotation());
	rotation.normalizeLocal();
	m_entityTransformInfo.setRotation(rotation);

	updateTransforms();
}

void PhysicalActor::scale(const Vector3f& scaleFactor)
{
	m_entityTransformInfo.setScale(m_entityTransformInfo.getScale().mul(scaleFactor));

	updateTransforms();
}

void PhysicalActor::scale(const float32 x, const float32 y, const float32 z)
{
	scale(Vector3f(x, y, z));
}

void PhysicalActor::scale(const float32 scaleFactor)
{
	scale(Vector3f(scaleFactor));
}

const Transform* PhysicalActor::getLocalToWorldTransform() const
{
	return &m_localToWorld;
}

const Transform* PhysicalActor::getWorldToLocalTransform() const
{
	return &m_worldToLocal;
}

void PhysicalActor::updateTransforms()
{
	m_localToWorld = m_entityTransformInfo.genTransform(Transform());
	m_worldToLocal = m_entityTransformInfo.genInverseTransform(Transform());
}

void swap(PhysicalActor& first, PhysicalActor& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<Actor&>(first),  static_cast<Actor&>(second));
	swap(first.m_entityTransformInfo, second.m_entityTransformInfo);
	swap(first.m_localToWorld,        second.m_localToWorld);
	swap(first.m_worldToLocal,        second.m_worldToLocal);
}

PhysicalActor& PhysicalActor::operator = (const PhysicalActor& rhs)
{
	m_entityTransformInfo = rhs.m_entityTransformInfo;
	m_localToWorld        = rhs.m_localToWorld;
	m_worldToLocal        = rhs.m_worldToLocal;

	return *this;
}

}// end namespace ph