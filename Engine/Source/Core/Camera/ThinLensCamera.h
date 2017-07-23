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

	virtual void genSensingRay(const Vector2R& rasterPosPx, Ray* const out_ray) const override;
	virtual void evalEmittedImportanceAndPdfW(
		const Vector3R& targetPos, 
		Vector2R* const out_filmCoord,
		Vector3R* const out_importance, 
		real* out_filmArea, 
		real* const out_pdfW) const override;

private:
	float32 m_lensRadiusMM;
	float32 m_focalDistanceMM;

	static void genRandomSampleOnDisk(const real radius, real* const out_x, real* const out_y);

// command interface
public:
	ThinLensCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<ThinLensCamera> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<ThinLensCamera>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph