#pragma once

#include "DataIO/SDL/TCommandInterface.h"
#include "Core/Receiver/PerspectiveReceiver.h"

namespace ph
{

class PinholeCamera : public PerspectiveReceiver, public TCommandInterface<PinholeCamera>
{
public:
	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;
	void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	const math::Vector3R& getPinholePos() const;
	math::Vector3R genReceiveRayDir(const math::Vector2D& rasterCoord) const;

// command interface
public:
	explicit PinholeCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

// In-header Implementations:

inline const math::Vector3R& PinholeCamera::getPinholePos() const
{
	return getPosition();
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  receiver             </category>
	<type_name> pinhole              </type_name>
	<extend>    receiver.perspective </extend>

	<name> Pinhole Camera </name>
	<description>
		This type of camera is simply composed of a hole (which serves as its lens system) and a 
		film. Images captured by this camera is similar to how a normal human perceives the world 
		but with several simplifications.
	</description>

	<command type="creator"/>

	</SDL_interface>
*/
