#pragma once

#include "Core/Receiver/Receiver.h"
#include "DataIO/SDL/TCommandInterface.h"
#include "Math/Transform/StaticRigidTransform.h"

namespace ph
{

class EnvironmentCamera : public Receiver, public TCommandInterface<EnvironmentCamera>
{
public:
	EnvironmentCamera();

	EnvironmentCamera(
		const math::Vector3R&    position,
		const math::QuaternionR& rotation,
		const math::Vector2S&    resolution);

	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;

	void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	math::StaticRigidTransform m_receiverToWorld;

// command interface
public:
	explicit EnvironmentCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  receiver          </category>
	<type_name> environment       </type_name>
	<extend>    receiver.receiver </extend>

	<name> Environment Camera </name>
	<description>
		This is an ideal 360-degree camera that took images of its environment
		in latitude-longitude format.
	</description>

	<command type="creator"/>

	</SDL_interface>
*/
