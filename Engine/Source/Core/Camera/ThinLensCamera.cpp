#include "Core/Camera/ThinLensCamera.h"
#include "Core/Camera/Film.h"
#include "Core/Sample.h"
#include "Core/Ray.h"

#include <iostream>

namespace ph
{

ThinLensCamera::ThinLensCamera(const float32 fov, const float32 lensRadius, const float32 lensFocalLength) :
	Camera(),
	m_fov(fov), m_lensRadius(lensRadius), m_lensFocalLength(lensFocalLength)
{

}

ThinLensCamera::~ThinLensCamera() = default;

void ThinLensCamera::genSensingRay(const Sample& sample, Ray* const out_ray) const
{
	const real aspectRatio = static_cast<real>(getFilm()->getWidthPx()) / static_cast<real>(getFilm()->getHeightPx());

	Vector3R rightDir = getDirection().cross(getUpAxis()).normalizeLocal();
	Vector3R upDir = rightDir.cross(getDirection()).normalizeLocal();

	const real halfWidth = std::tan(m_fov / 2.0_r);
	const real halfHeight = halfWidth / aspectRatio;

	const real pixelPosX = sample.m_cameraX * halfWidth;
	const real pixelPosY = sample.m_cameraY * halfHeight;

	rightDir.mulLocal(pixelPosX);
	upDir.mulLocal(pixelPosY);

	out_ray->setDirection(getDirection().add(rightDir.addLocal(upDir)).mulLocal(-1.0_r).normalizeLocal());
	out_ray->setOrigin(getPosition());
	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
	out_ray->setMaxT(Ray::MAX_T);
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