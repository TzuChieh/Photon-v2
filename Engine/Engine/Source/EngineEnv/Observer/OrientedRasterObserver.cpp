#include "EngineEnv/Observer/OrientedRasterObserver.h"
#include "Math/math.h"
#include "Math/TMatrix4.h"

#include <Common/logging.h>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(OrientedRasterObserver, Observer);

math::TDecomposedTransform<float64> OrientedRasterObserver::makeObserverPose() const
{
	math::TDecomposedTransform<float64> pose;
	pose.setPos(makePos());
	pose.setRot(makeRot());
	return pose;
}

math::Vector3D OrientedRasterObserver::makePos() const
{
	return math::Vector3D(m_pos);
}

math::QuaternionD OrientedRasterObserver::makeRot() const
{
	auto rotation = math::QuaternionD::makeNoRotation();
	if(m_dir && m_upAxis)
	{
		rotation = makeRotFromVectors(*m_dir, *m_upAxis);
	}
	else
	{
		rotation = makeRotFromYawPitchRoll(
			m_yawPitchRollDegrees.x(),
			m_yawPitchRollDegrees.y(),
			m_yawPitchRollDegrees.z());
	}

	return rotation;
}

math::Vector3D OrientedRasterObserver::makeDir() const
{
	return math::Vector3D(0, 0, -1).rotate(makeRot()).normalize();
}

math::QuaternionD OrientedRasterObserver::makeRotFromVectors(
	const math::Vector3R& dir, 
	const math::Vector3R& upAxis)
{
	constexpr float64 MIN_LENGTH = 0.001;

	// Projective observers face the negated z-axis of its basis by default,
	// negate again to get the z-axis back
	//
	auto zAxis = math::Vector3D(dir).mul(-1);
	if(zAxis.lengthSquared() > MIN_LENGTH * MIN_LENGTH)
	{
		zAxis.normalizeLocal();
	}
	else
	{
		PH_LOG(OrientedRasterObserver, Warning, 
			"Direction vector {} is too short. Defaults to -z axis.", dir.toString());

		zAxis.set({0, 0, 1});
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
		PH_LOG(OrientedRasterObserver, Warning,
			"Up axis {} is not properly configured. It is too close to the direction "
			"vector, or its length is too short. Trying to recover using +x as right "
			"vector or +y as up vector.",
			upAxis.toString());

		if(zAxis.absDot({1, 0, 0}) < zAxis.absDot({0, 1, 0}))
		{
			yAxis = zAxis.cross({1, 0, 0}).normalizeLocal();
			xAxis = yAxis.cross(zAxis).normalizeLocal();
		}
		else
		{
			xAxis = math::Vector3D(0, 1, 0).cross(zAxis).normalizeLocal();
			yAxis = zAxis.cross(xAxis).normalizeLocal();
		}
	}
	
	const auto worldToViewRotMat = math::Matrix4D().initRotation(xAxis, yAxis, zAxis);
	return math::QuaternionD(worldToViewRotMat);
}

math::QuaternionD OrientedRasterObserver::makeRotFromYawPitchRoll(
	real yawDegrees,
	real pitchDegrees,
	real rollDegrees)
{
	if(yawDegrees < -180.0_r || yawDegrees > 180.0_r)
	{
		PH_LOG(OrientedRasterObserver, Warning,
			"Yaw degrees {} out of range. Clamping to [-180, 180].", yawDegrees);

		yawDegrees = math::clamp(yawDegrees, -180.0_r, 180.0_r);
	}

	if(pitchDegrees < -90.0_r || pitchDegrees > 90.0_r)
	{
		PH_LOG(OrientedRasterObserver, Warning,
			"Pitch degrees {} out of range. Clamping to [-90, 90].", pitchDegrees);

		pitchDegrees = math::clamp(pitchDegrees, -90.0_r, 90.0_r);
	}

	if(rollDegrees < -180.0_r || rollDegrees > 180.0_r)
	{
		PH_LOG(OrientedRasterObserver, Warning,
			"Roll degrees {} out of range. Clamping to [-180, 180].", yawDegrees);

		rollDegrees = math::clamp(rollDegrees, -180.0_r, 180.0_r);
	}

	const math::QuaternionD yawRot({0, 1, 0}, math::to_radians(yawDegrees));
	const math::QuaternionD pitchRot({1, 0, 0}, math::to_radians(pitchDegrees));
	const math::QuaternionD rollRot({0, 0, 1}, math::to_radians(rollDegrees));
	return yawRot.mul(pitchRot).mul(rollRot).normalize();
}

}// end namespace ph
