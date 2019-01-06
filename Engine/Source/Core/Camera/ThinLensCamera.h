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