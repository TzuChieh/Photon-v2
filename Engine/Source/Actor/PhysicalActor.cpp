#include "Actor/PhysicalActor.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/math.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/InputPrototype.h"

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

void PhysicalActor::setBaseTransform(const TDecomposedTransform<hiReal>& baseLocalToWorld)
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
	// FIXME: base?

	m_localToWorld = rhs.m_localToWorld;

	return *this;
}

void swap(PhysicalActor& first, PhysicalActor& second)
{
	// enable ADL
	using std::swap;

	// by swapping the members of two objects, the two objects are effectively swapped
	swap(static_cast<Actor&>(first), static_cast<Actor&>(second));
	swap(first.m_localToWorld,       second.m_localToWorld);
}

// command interface

PhysicalActor::PhysicalActor(const InputPacket& packet) : 
	Actor(packet),
	m_localToWorld()
{}

SdlTypeInfo PhysicalActor::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_ACTOR, "physical");
}

void PhysicalActor::ciRegister(CommandRegister& cmdRegister)
{
	SdlExecutor translateSE;
	translateSE.setName("translate");
	translateSE.setFunc<PhysicalActor>(ciTranslate);
	cmdRegister.addExecutor(translateSE);

	SdlExecutor rotateSE;
	rotateSE.setName("rotate");
	rotateSE.setFunc<PhysicalActor>(ciRotate);
	cmdRegister.addExecutor(rotateSE);

	SdlExecutor scaleSE;
	scaleSE.setName("scale");
	scaleSE.setFunc<PhysicalActor>(ciScale);
	cmdRegister.addExecutor(scaleSE);
}

ExitStatus PhysicalActor::ciTranslate(
	const std::shared_ptr<PhysicalActor>& targetResource,
	const InputPacket& packet)
{
	InputPrototype translationInput;
	translationInput.addVector3("factor");

	if(packet.isPrototypeMatched(translationInput))
	{
		const Vector3R translation = packet.getVector3("factor");
		targetResource->translate(translation);
		return ExitStatus::SUCCESS();
	}
	else
	{
		return ExitStatus::BAD_INPUT("requiring a vector3 factor");
	}
}

ExitStatus PhysicalActor::ciRotate(
	const std::shared_ptr<PhysicalActor>& targetResource,
	const InputPacket& packet)
{
	InputPrototype quaternionInput;
	quaternionInput.addQuaternion("factor");

	InputPrototype axisDegreeInput;
	axisDegreeInput.addVector3("axis");
	axisDegreeInput.addReal("degree");

	if(packet.isPrototypeMatched(quaternionInput))
	{
		const QuaternionR rotation = packet.getQuaternion("factor");
		targetResource->rotate(rotation.normalize());
		return ExitStatus::SUCCESS();
	}
	else if(packet.isPrototypeMatched(axisDegreeInput))
	{
		const Vector3R axis = packet.getVector3("axis");
		const real     degrees = packet.getReal("degree");
		targetResource->rotate(axis.normalize(), degrees);
		return ExitStatus::SUCCESS();
	}
	else
	{
		return ExitStatus::BAD_INPUT(std::string()
			+ "possible input formats are: "
			+ "1. " + quaternionInput.toString() + "; "
			+ "2. " + axisDegreeInput.toString());
	}
}

ExitStatus PhysicalActor::ciScale(
	const std::shared_ptr<PhysicalActor>& targetResource,
	const InputPacket& packet)
{
	InputPrototype scalationInput;
	scalationInput.addVector3("factor");

	if(packet.isPrototypeMatched(scalationInput))
	{
		const Vector3R scalation = packet.getVector3("factor");
		targetResource->scale(scalation);
		return ExitStatus::SUCCESS();
	}
	else
	{
		return ExitStatus::BAD_INPUT("requiring a vector3 factor");
	}
}

}// end namespace ph