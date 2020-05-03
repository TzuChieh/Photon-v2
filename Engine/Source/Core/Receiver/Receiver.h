#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Math/TQuaternion.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "Math/Transform/TDecomposedTransform.h"
#include "Core/Filmic/filmic_fwd.h"
#include "Common/assertion.h"
#include "Core/Quantity/SpectralStrength.h"

namespace ph
{

class Ray;
class InputPacket;
class RayDifferential;

class Receiver : public TCommandInterface<Receiver>
{
public:
	Receiver();

	Receiver(
		const math::Vector3R&    position, 
		const math::QuaternionR& rotation,
		const math::Vector2S&    resolution);

	virtual ~Receiver() = default;

	/*! @brief Generate a ray received by the receiver.

	Given raster coordinates, generate a corresponding ray that would have
	hit that location from the light entry of the receiver. The light entry
	would be the outmost surface of a camera's lens system for example.

	@return A weighting factor for the received quantity.
	*/
	virtual SpectralStrength receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const = 0;

	// Given a ray generated by receiveRay() along with the parameters for it, 
	// calculates differential information on the origin of the ray.
	// The default implementation uses numerical differentiation for 
	// the differentials.
	/*virtual void calcSensedRayDifferentials(const math::Vector2R& filmNdcPos, const Ray& sensedRay,
	                                        RayDifferential* out_result) const;*/

	virtual void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const = 0;

	const math::Vector3R& getPosition() const;
	const math::Vector3R& getDirection() const;
	const math::Vector2S& getRasterResolution() const;
	float64 getAspectRatio() const;

protected:
	math::Vector3R                      m_position;
	math::Vector3R                      m_direction;
	math::Vector2S                      m_resolution;
	math::TDecomposedTransform<float64> m_receiverToWorldDecomposed;

private:
	static math::Vector3R makeDirectionFromRotation(const math::QuaternionR& rotation);
	static math::QuaternionR makeRotationFromYawPitch(real yawDegrees, real pitchDegrees);

	static math::QuaternionR makeRotationFromVectors(
		const math::Vector3R& direction, 
		const math::Vector3R& upAxis);

	static math::TDecomposedTransform<float64> makeDecomposedReceiverPose(
		const math::Vector3R&    position, 
		const math::QuaternionR& rotation);

// command interface
public:
	explicit Receiver(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline const math::Vector3R& Receiver::getPosition() const
{
	return m_position;
}

inline const math::Vector3R& Receiver::getDirection() const
{
	return m_direction;
}

inline const math::Vector2S& Receiver::getRasterResolution() const
{
	return m_resolution;
}

inline float64 Receiver::getAspectRatio() const
{
	PH_ASSERT_GT(m_resolution.y, 0);

	return static_cast<float64>(m_resolution.x) / static_cast<float64>(m_resolution.y);
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  receiver </category>
	<type_name> receiver </type_name>

	<name> Receiver </name>
	<description>
		An energy receiver for observing the scene.
	</description>

	<command type="creator" intent="blueprint">

		<input name="position" type="vector3">
			<description>Position of the receiver.</description>
		</input>

		<input name="rotation" type="quaternion">
			<description>The orientation of the receiver.</description>
		</input>

		<input name="direction" type="vector3">
			<description>Direction that this receiver is looking at.</description>
		</input>
		<input name="up-axis" type="vector3">
			<description>The direction that this receiver consider as upward.</description>
		</input>

		<input name="yaw-degrees" type="real">
			<description>Rotation of the receiver around +y axis in [0, 360].</description>
		</input>
		<input name="pitch-degrees" type="real">
			<description>The receiver's declination from the horizon in [-90, 90].</description>
		</input>

	</command>

	</SDL_interface>
*/