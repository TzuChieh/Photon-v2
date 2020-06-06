#pragma once

#include "Core/Receiver/Receiver.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "Math/Transform/TDecomposedTransform.h"

#include <iostream>

namespace ph
{

class Ray;
class InputPacket;

namespace math
{
	class Transform;
}

class PerspectiveReceiver : public Receiver, public TCommandInterface<PerspectiveReceiver>
{
public:
	// TODO: ordinary ctors

	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override = 0;
	void evalEmittedImportanceAndPdfW(
		const math::Vector3R& targetPos,
		math::Vector2R* const out_filmCoord,
		math::Vector3R* const out_importance,
		real* out_filmArea, 
		real* const out_pdfW) const override = 0;

protected:
	std::shared_ptr<math::Transform>    m_receiverToWorld;// FIXME: should be rigid
	std::shared_ptr<math::Transform>    m_rasterToReceiver;
	math::TDecomposedTransform<float64> m_rasterToReceiverDecomposed;
	
private:
	float64 m_sensorWidth;
	float64 m_sensorOffset;

	void updateTransforms();

// command interface
public:
	explicit PerspectiveReceiver(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  receiver          </category>
	<type_name> perspective       </type_name>
	<extend>    receiver.receiver </extend>

	<name> Perspective Receiver </name>
	<description>
		For receivers that creates perspective effect in their resulting image.
	</description>

	<command type="creator" intent="blueprint">
		<input name="fov-degree" type="real">
			<description>Field of view of this receiver in degrees.</description>
		</input>
		<input name="sensor-width-mm" type="real">
			<description>Width of the sensor used by this receiver in millimeters.</description>
		</input>
		<input name="sensor-offset-mm" type="real">
			<description>Distance between sensor and light entry.</description>
		</input>
	</command>

	</SDL_interface>
*/
