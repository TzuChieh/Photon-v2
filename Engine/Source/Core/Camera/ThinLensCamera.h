#pragma once

#include "Core/Camera/Camera.h"
#include "FileIO/SDL/ISdlResource.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class ThinLensCamera : public Camera, public TCommandInterface<ThinLensCamera>
{
public:
	virtual ~ThinLensCamera() override;

	virtual void genSensingRay(const Sample& sample, Ray* const out_ray) const override;
	virtual void evalEmittedImportanceAndPdfW(
		const Vector3R& targetPos, 
		Vector2f* const out_filmCoord, 
		Vector3R* const out_importance, 
		real* out_filmArea, 
		real* const out_pdfW) const override;

// command interface
public:
	ThinLensCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<ThinLensCamera>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph