#include "Core/Camera/ThinLensCamera.h"

#include <iostream>

namespace ph
{

ThinLensCamera::~ThinLensCamera() = default;

void ThinLensCamera::genSensingRay(const Sample& sample, Ray* const out_ray) const
{

}

void ThinLensCamera::evalEmittedImportanceAndPdfW(
	const Vector3R& targetPos,
	Vector2f* const out_filmCoord,
	Vector3R* const out_importance,
	real* out_filmArea,
	real* const out_pdfW) const
{
	std::cerr << "ThinLensCamera::evalEmittedImportanceAndPdfW() not implemented" << std::endl;
}

// command interface

ThinLensCamera::ThinLensCamera(const InputPacket& packet) : 
	Camera(packet)
{

}

SdlTypeInfo ThinLensCamera::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_CAMERA, "thin-lens");
}

ExitStatus ThinLensCamera::ciExecute(const std::shared_ptr<ThinLensCamera>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph