#include "Core/Camera/ThinLensCamera.h"
#include "Core/Sample.h"
#include "Core/Ray.h"
#include "Math/Transform/Transform.h"
#include "FileIO/SDL/InputPacket.h"
#include "Math/Random.h"

#include <iostream>

namespace ph
{

void ThinLensCamera::genSensedRay(const Vector2R& filmNdcPos, Ray* const out_ray) const
{
	Vector3R camFilmPos;
	m_filmToCamera->transformP(Vector3R(filmNdcPos.x, filmNdcPos.y, 0), &camFilmPos);

	const Vector3R camCenterRayDir = camFilmPos.mul(-1);
	const real     hitParamDist    = m_focalDistanceMM / (-camCenterRayDir.z);
	const Vector3R camFocusPos     = camCenterRayDir.mul(hitParamDist);

	Vector3R camLensPos;
	genRandomSampleOnDisk(m_lensRadiusMM, &camLensPos.x, &camLensPos.y);

	Vector3R worldLensPos, worldFocusPos;
	m_cameraToWorld->transformP(camLensPos,  &worldLensPos);
	m_cameraToWorld->transformP(camFocusPos, &worldFocusPos);


	out_ray->setDirection(worldLensPos.sub(worldFocusPos).normalizeLocal());
	out_ray->setOrigin(worldLensPos);
	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
	out_ray->setMaxT(std::numeric_limits<real>::max());
}

void ThinLensCamera::evalEmittedImportanceAndPdfW(
	const Vector3R& targetPos,
	Vector2R* const out_filmCoord,
	Vector3R* const out_importance,
	real* out_filmArea,
	real* const out_pdfW) const
{
	std::cerr << "ThinLensCamera::evalEmittedImportanceAndPdfW() not implemented" << std::endl;
}

void ThinLensCamera::genRandomSampleOnDisk(const real radius, real* const out_x, real* const out_y)
{
	const real r   = radius * std::sqrt(Random::genUniformReal_i0_e1());
	const real phi = 2.0_r * PH_PI_REAL * Random::genUniformReal_i0_e1();
	*out_x = r * std::cos(phi);
	*out_y = r * std::sin(phi);
}

// command interface

ThinLensCamera::ThinLensCamera(const InputPacket& packet) : 
	PerspectiveCamera(packet), 
	m_lensRadiusMM(0.0_r), m_focalDistanceMM()
{
	m_lensRadiusMM    = packet.getReal("lens-radius-mm",    m_lensRadiusMM,    DataTreatment::REQUIRED());
	m_focalDistanceMM = packet.getReal("focal-distance-mm", m_focalDistanceMM, DataTreatment::REQUIRED());
}

SdlTypeInfo ThinLensCamera::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_CAMERA, "thin-lens");
}

void ThinLensCamera::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<ThinLensCamera>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<ThinLensCamera> ThinLensCamera::ciLoad(const InputPacket& packet)
{
	return std::make_unique<ThinLensCamera>(packet);
}

}// end namespace ph