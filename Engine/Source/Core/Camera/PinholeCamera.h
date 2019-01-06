#pragma once

#include "Core/Camera/Camera.h"
#include "Common/primitive_type.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Camera/PerspectiveCamera.h"

namespace ph
{

class PinholeCamera : public PerspectiveCamera, public TCommandInterface<PinholeCamera>
{
public:
	void genSensedRay(const Vector2R& filmNdcPos, Ray* out_ray) const override;
	void evalEmittedImportanceAndPdfW(const Vector3R& targetPos, Vector2R* const out_filmCoord, Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	const Vector3R& getPinholePos() const;
	Vector3R genSensedRayDir(const Vector2R& filmNdcPos) const;

// command interface
public:
	explicit PinholeCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<PinholeCamera> ciLoad(const InputPacket& packet);
};

// In-header Implementations:

inline const Vector3R& PinholeCamera::getPinholePos() const
{
	return getPosition();
}

}// end namespace ph

/*
	<SDL_interface>

	<category>  camera             </category>
	<type_name> pinhole            </type_name>
	<extend>    camera.perspective </extend>

	<name> Pinhole Camera </name>
	<description>
		This type of camera is simply composed of a hole (which serves as its lens system) and a 
		film. Images captured by this camera is similar to how a normal human perceives the world 
		but with several simplifications.
	</description>

	<command type="creator"/>

	</SDL_interface>
*/