#include "Core/Receiver/Receiver.h"
#include "DataIO/SDL/InputPacket.h"
#include "Core/RayDifferential.h"
#include "Core/Ray.h"
#include "Common/Logger.h"
#include "Math/math.h"

#include <iostream>
#include <string>

namespace ph
{

namespace
{
	const Logger logger(LogSender("Receiver"));
}

Receiver::Receiver() :
	Receiver(
		math::Vector3R(0, 0, 0), 
		math::QuaternionR::makeNoRotation(),
		{960, 540})
{}

Receiver::Receiver(
	const math::Vector3R&    position, 
	const math::QuaternionR& rotation,
	const math::Vector2S&    resolution) :

	m_position                 (position),
	m_direction                (makeDirectionFromRotation(rotation)),
	m_resolution               (resolution),
	m_receiverToWorldDecomposed(makeDecomposedReceiverPose(position, rotation))
{
	PH_ASSERT_GT(m_resolution.x, 0);
	PH_ASSERT_GT(m_resolution.y, 0);
}

//void Camera::calcSensedRayDifferentials(
//	const math::Vector2R& rasterPosPx, const Ray& sensedRay,
//	RayDifferential* const out_result) const
//{
//	// 2nd-order accurate with respect to the size of <deltaPx>
//	const real deltaPx        = 1.0_r / 32.0_r;
//	const real reciIntervalPx = 1.0_r / deltaPx;
//
//	Ray dnxRay, dpxRay, dnyRay, dpyRay;
//	receiveRay(math::Vector2R(rasterPosPx.x - deltaPx, rasterPosPx.y), &dnxRay);
//	receiveRay(math::Vector2R(rasterPosPx.x + deltaPx, rasterPosPx.y), &dpxRay);
//	receiveRay(math::Vector2R(rasterPosPx.x, rasterPosPx.y - deltaPx), &dnyRay);
//	receiveRay(math::Vector2R(rasterPosPx.x, rasterPosPx.y + deltaPx), &dpyRay);
//
//	out_result->setPartialPs((dpxRay.getOrigin() - dnxRay.getOrigin()).divLocal(reciIntervalPx),
//	                         (dpyRay.getOrigin() - dnyRay.getOrigin()).divLocal(reciIntervalPx));
//
//	out_result->setPartialDs((dpxRay.getDirection() - dnxRay.getDirection()).divLocal(reciIntervalPx),
//	                         (dpyRay.getDirection() - dnyRay.getDirection()).divLocal(reciIntervalPx));
//}

math::Vector3R Receiver::makeDirectionFromRotation(const math::QuaternionR& rotation)
{
	return math::Vector3R(0, 0, -1).rotate(rotation).normalize();
}

math::TDecomposedTransform<float64> Receiver::makeDecomposedReceiverPose(
	const math::Vector3R&    position,
	const math::QuaternionR& rotation)
{
	math::TDecomposedTransform<float64> pose;

	pose.setPosition(math::Vector3D(position));

	// Changes unit from mm to m
	pose.setScale(0.001);

	pose.setRotation(math::QuaternionD(rotation));

	return pose;
}

math::QuaternionR Receiver::makeRotationFromVectors(
	const math::Vector3R& direction,
	const math::Vector3R& upAxis)
{
	constexpr real MIN_LENGTH = 0.001_r;

	auto zAxis = math::Vector3R(direction).mul(-1);
	if(zAxis.lengthSquared() > MIN_LENGTH * MIN_LENGTH)
	{
		zAxis.normalizeLocal();
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"Direction vector " + direction.toString() + " is too short. "
			"Defaults to -z axis.");

		zAxis.set(0, 0, -1);
	}

	auto xAxis = math::Vector3R(upAxis).cross(zAxis);
	auto yAxis = zAxis.cross(xAxis);
	if(xAxis.lengthSquared() > MIN_LENGTH * MIN_LENGTH)
	{
		xAxis.normalizeLocal();
		yAxis.normalizeLocal();
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"Up axis " + upAxis.toString() + "is not properly configured. "
			"It is too close to the direction vector, or its length is too short. "
			"Trying to recover using +x as right vector or +y as up vector.");

		if(zAxis.absDot({1, 0, 0}) < zAxis.absDot({0, 1, 0}))
		{
			yAxis = zAxis.cross({1, 0, 0}).normalizeLocal();
			xAxis = yAxis.cross(zAxis).normalizeLocal();
		}
		else
		{
			xAxis = math::Vector3R(0, 1, 0).cross(zAxis).normalizeLocal();
			yAxis = zAxis.cross(xAxis).normalizeLocal();
		}
	}

	const auto worldToViewRotMat = math::Matrix4R().initRotation(xAxis, yAxis, zAxis);
	return math::QuaternionR(worldToViewRotMat);
}

math::QuaternionR Receiver::makeRotationFromYawPitch(real yawDegrees, real pitchDegrees)
{
	if(yawDegrees < 0.0_r || yawDegrees > 360.0_r)
	{
		logger.log(ELogLevel::WARNING_MED,
			"Yaw degree " + std::to_string(yawDegrees) + " out of range. Clamping to [0, 360]");

		yawDegrees = math::clamp(yawDegrees, 0.0_r, 360.0_r);
	}

	if(pitchDegrees < -90.0_r || pitchDegrees > 90.0_r)
	{
		logger.log(ELogLevel::WARNING_MED,
			"Pitch degree " + std::to_string(pitchDegrees) + " out of range. Clamping to [-90, 90]");

		pitchDegrees = math::clamp(pitchDegrees, -90.0_r, 90.0_r);
	}

	const math::QuaternionR yawRot({0, 1, 0}, math::to_radians(yawDegrees));
	const math::QuaternionR pitchRot({1, 0, 0}, math::to_radians(pitchDegrees));
	return yawRot.mul(pitchRot).normalize();
}

// command interface

Receiver::Receiver(const InputPacket& packet) :
	Receiver()
{
	m_position = packet.getVector3("position", 
		m_position, DataTreatment::REQUIRED());

	auto rotation = math::QuaternionR::makeNoRotation();
	if(packet.hasQuaternion("rotation"))
	{
		rotation = packet.getQuaternion("rotation");
	}
	else if(packet.hasVector3("direction") && packet.hasVector3("up-axis"))
	{
		const auto direction = packet.getVector3("direction");
		const auto upAxis    = packet.getVector3("up-axis");
		rotation = makeRotationFromVectors(direction, upAxis);
	}
	else if(packet.hasReal("yaw-degrees") && packet.hasReal("pitch-degrees"))
	{
		rotation = makeRotationFromYawPitch(packet.getReal("yaw-degrees"), packet.getReal("pitch-degrees"));
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED, 
			"no rotation info provided");
	}
	m_direction = makeDirectionFromRotation(rotation);

	m_resolution.x = packet.getInteger("resolution-x",
		static_cast<integer>(m_resolution.x), DataTreatment::REQUIRED("using default: " + std::to_string(m_resolution.x)));
	m_resolution.y = packet.getInteger("resolution-y",
		static_cast<integer>(m_resolution.y), DataTreatment::REQUIRED("using default: " + std::to_string(m_resolution.y)));

	m_receiverToWorldDecomposed = makeDecomposedReceiverPose(m_position, rotation);
}

SdlTypeInfo Receiver::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_RECEIVER, "receiver");
}

void Receiver::ciRegister(CommandRegister& cmdRegister)
{}

}// end namespace ph
