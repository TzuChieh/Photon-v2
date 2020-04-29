#pragma once

#include "Core/Camera/Camera.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "DataIO/SDL/ISdlResource.h"
#include "DataIO/SDL/TCommandInterface.h"

#include <iostream>

namespace ph
{

class Ray;
class Sample;
class SampleGenerator;
class Film;
class InputPacket;

class PerspectiveCamera : public Camera, public TCommandInterface<PerspectiveCamera>
{
public:
	void genSensedRay(const math::Vector2R& filmNdcPos, Ray* out_ray) const override = 0;
	void evalEmittedImportanceAndPdfW(
		const math::Vector3R& targetPos,
		math::Vector2R* const out_filmCoord,
		math::Vector3R* const out_importance,
		real* out_filmArea, 
		real* const out_pdfW) const override = 0;

protected:
	std::shared_ptr<math::Transform> m_cameraToWorld;
	std::shared_ptr<math::Transform> m_filmToCamera;
	std::shared_ptr<math::Transform> m_filmToWorld;
	
private:
	real m_filmWidthMM;
	real m_filmOffsetMM;

	void updateTransforms();

// command interface
public:
	explicit PerspectiveCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph

/*
	<SDL_interface>

	<category>  camera        </category>
	<type_name> perspective   </type_name>
	<extend>    camera.camera </extend>

	<name> Perspective Camera </name>
	<description>
		For cameras that have perspective effect.
	</description>

	<command type="creator" intent="blueprint">
		<input name="fov-degree" type="real">
			<description>Field of view of this camera in degrees.</description>
		</input>
		<input name="film-width-mm" type="real">
			<description>Width of the film used by this camera in millimeters.</description>
		</input>
		<input name="film-offset-mm" type="real">
			<description>Distance from the film to the camera's lens.</description>
		</input>
	</command>

	</SDL_interface>
*/
