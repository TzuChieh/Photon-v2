#include "EngineEnv/Observer/OrientedRasterObserver.h"
#include "Common/Logger.h"
#include "Math/math.h"
#include "Math/TMatrix4.h"

namespace ph
{

namespace
{

const Logger logger(LogSender("Oriented Raster Observer"));

}

math::TDecomposedTransform<float64> OrientedRasterObserver::makeObserverPose() const
{
	math::TDecomposedTransform<float64> pose;
	pose.setPosition(makePosition());
	pose.setRotation(makeRotation());
	return pose;
}

math::Vector3D OrientedRasterObserver::makePosition() const
{
	return math::Vector3D(m_position);
}

math::QuaternionD OrientedRasterObserver::makeRotation() const
{
	auto rotation = math::QuaternionD::makeNoRotation();
	if(m_direction && m_upAxis)
	{
		rotation = makeRotationFromVectors(*m_direction, *m_upAxis);
	}
	else
	{
		rotation = makeRotationFromYawPitchRoll(
			m_yawPitchRollDegrees.x,
			m_yawPitchRollDegrees.y,
			m_yawPitchRollDegrees.z);
	}

	return rotation;
}

math::Vector3D OrientedRasterObserver::makeDirection() const
{
	return math::Vector3D(0, 0, -1).rotate(makeRotation()).normalize();
}

math::QuaternionD OrientedRasterObserver::makeRotationFromVectors(
	const math::Vector3R& direction, 
	const math::Vector3R& upAxis)
{
	constexpr float64 MIN_LENGTH = 0.001;

	// Projective observers face the negated z-axis of its basis by default,
	// negate again to get the z-axis back
	//
	auto zAxis = math::Vector3D(direction).mul(-1);
	if(zAxis.lengthSquared() > MIN_LENGTH * MIN_LENGTH)
	{
		zAxis.normalizeLocal();
	}
	else
	{
		logger.log(ELogLevel::WARNING_MED,
			"Direction vector " + direction.toString() + " is too short. "
			"Defaults to -z axis.");

		zAxis.set(0, 0, 1);
	}

	auto xAxis = math::Vector3D(upAxis).cross(zAxis);
	auto yAxis = zAxis.cross(xAxis);
	if(xAxis.lengthSquared() > MIN_LENGTH * MIN_LENGTH)
	{
		xAxis.normalizeLocal();
		yAxis.normalizeLocal();
	}
	else
	{
		// TODO: make this a note not warning as this can be properly recovered
		//       (warn on short up-axis though)
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
	
	const auto worldToViewRotMat = math::Matrix4D().initRotation(xAxis, yAxis, zAxis);
	return math::QuaternionD(worldToViewRotMat);
}

math::QuaternionD OrientedRasterObserver::makeRotationFromYawPitchRoll(
	const real yawDegrees,
	const real pitchDegrees,
	const real rollDegrees)
{
	if(yawDegrees < -180.0_r || yawDegrees > 180.0_r)
	{
		logger.log(ELogLevel::WARNING_MED,
			"Yaw degrees " + std::to_string(yawDegrees) + " out of range. Clamping to [-180, 180]");

		yawDegrees = math::clamp(yawDegrees, -180.0_r, 180.0_r);
	}

	if(pitchDegrees < -90.0_r || pitchDegrees > 90.0_r)
	{
		logger.log(ELogLevel::WARNING_MED,
			"Pitch degrees " + std::to_string(pitchDegrees) + " out of range. Clamping to [-90, 90]");

		pitchDegrees = math::clamp(pitchDegrees, -90.0_r, 90.0_r);
	}

	if(rollDegrees < -180.0_r || rollDegrees > 180.0_r)
	{
		logger.log(ELogLevel::WARNING_MED,
			"Roll degrees " + std::to_string(yawDegrees) + " out of range. Clamping to [-180, 180]");

		rollDegrees = math::clamp(rollDegrees, -180.0_r, 180.0_r);
	}

	const math::QuaternionD yawRot({0, 1, 0}, math::to_radians(yawDegrees));
	const math::QuaternionD pitchRot({1, 0, 0}, math::to_radians(pitchDegrees));
	const math::QuaternionD rollRot({0, 0, 1}, math::to_radians(rollDegrees));
	return yawRot.mul(pitchRot).mul(rollRot).normalize();
}

}// end namespace ph