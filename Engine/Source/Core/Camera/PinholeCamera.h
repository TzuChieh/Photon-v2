#pragma once

#include "Core/Camera/Camera.h"
#include "Common/primitive_type.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class PinholeCamera final : public Camera, public TCommandInterface<PinholeCamera>
{
public:
	PinholeCamera();
	virtual ~PinholeCamera() override;

	virtual void genSensingRay(const Sample& sample, Ray* const out_ray) const override;
	virtual void evalEmittedImportanceAndPdfW(const Vector3R& targetPos, Vector2f* const out_filmCoord, Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	real m_fov;

// command interface
public:
	PinholeCamera(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static ExitStatus ciExecute(const std::shared_ptr<PinholeCamera>& targetResource, const std::string& functionName, const InputPacket& packet);
};

}// end namespace ph