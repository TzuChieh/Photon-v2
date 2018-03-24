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
	virtual ~ThinLensCamera() override;

	virtual void genSensedRay(const Vector2R& rasterPosPx, Ray* out_ray) const override;
	virtual void evalEmittedImportanceAndPdfW(
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
	ThinLensCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<ThinLensCamera> ciLoad(const InputPacket& packet);
};

}// end namespace ph