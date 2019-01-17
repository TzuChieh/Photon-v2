#pragma once

#include "Core/Camera/Camera.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/Camera/PerspectiveCamera.h"

namespace ph
{

class ThinLensCamera : public PerspectiveCamera, public TCommandInterface<ThinLensCamera>
{
public:
	void genSensedRay(const Vector2R& filmNdcPos, Ray* out_ray) const override;
	void evalEmittedImportanceAndPdfW(
		const Vector3R& targetPos, 
		Vector2R* const out_filmCoord,
		Vector3R* const out_importance, 
		real* out_filmArea, 
		real* const out_pdfW) const override;

private:
	real m_lensRadiusMM;
	real m_focalDistanceMM;

	static void genRandomSampleOnDisk(const real radius, real* const out_x, real* const out_y);

// command interface
public:
	explicit ThinLensCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  camera             </category>
	<type_name> thin-lens          </type_name>
	<extend>    camera.perspective </extend>

	<name> Thin Lens Camera </name>
	<description>
		As its name suggests, the lens system in this camera is assumed to be 
		a single lens with negligible thickness. The biggest advantage of it 
		is that depth of field effects are possible under this model.
	</description>

	<command type="creator">

		<input name="lens-radius-mm" type="real">
			<description>Radius of the lens in millimeters.</description>
		</input>

		<input name="focal-distance-mm" type="real">
			<description>The distance in millimeters that the camera is focusing on.</description>
		</input>

	</command>

	</SDL_interface>
*/