#include "Core/Camera/PinholeCamera.h"
#include "Core/Ray.h"
#include "Core/Sample.h"
#include "Core/Camera/Film.h"
#include "FileIO/InputPacket.h"
#include "Math/Math.h"

#include <limits>

namespace ph
{

PinholeCamera::~PinholeCamera() = default;

void PinholeCamera::genSensingRay(const Sample& sample, Ray* const out_ray) const
{
	const Vector3R rasterPosPx(sample.m_cameraX * getFilm()->getWidthPx(),
	                           sample.m_cameraY * getFilm()->getHeightPx(),
	                           0);
	const Vector3R& worldPinholePos = getPosition();

	Vector3R worldFilmPos;
	m_rasterToWorld->transformP(rasterPosPx, &worldFilmPos);

	out_ray->setDirection(worldFilmPos.subLocal(worldPinholePos).normalizeLocal());
	out_ray->setOrigin(getPosition());
	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
	out_ray->setMaxT(Ray::MAX_T);
}

void PinholeCamera::evalEmittedImportanceAndPdfW(const Vector3R& targetPos, Vector2f* const out_filmCoord, Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const
{
	std::cerr << "PinholeCamera::evalEmittedImportanceAndPdfW() not implemented" << std::endl;

	/*const Vector3R targetDir = targetPos.sub(getPosition()).normalizeLocal();
	const real cosTheta = targetDir.dot(getDirection());

	if(cosTheta <= 0.0_r)
	{
		out_importance->set(0.0_r);
		*out_pdfW = 0.0_r;
		return;
	}

	const Vector3R filmPos = targetDir.mul(1.0_r / cosTheta).add(getPosition());
	const Vector3R filmCenter = getDirection().add(getPosition());
	const Vector3R filmVec = filmPos.sub(filmCenter);

	const Vector3R rightDir = getDirection().cross(getUpAxis()).normalizeLocal();
	const Vector3R upDir    = rightDir.cross(getDirection()).normalizeLocal();

	const real aspectRatio = static_cast<real>(getFilm()->getWidthPx()) / static_cast<real>(getFilm()->getHeightPx());
	const real halfFilmWidth = std::tan(m_fov / 2.0_r);
	const real halfFilmHeight = halfFilmWidth / aspectRatio;

	out_filmCoord->x = filmVec.dot(rightDir) / halfFilmWidth;
	out_filmCoord->y = filmVec.dot(upDir) / halfFilmHeight;

	if(std::abs(out_filmCoord->x) > 1.0_r || std::abs(out_filmCoord->y) > 1.0_r)
	{
		out_importance->set(0.0_r);
		*out_pdfW = 0.0_r;
		return;
	}

	*out_filmArea = halfFilmWidth * halfFilmHeight * 4.0_r;
	out_importance->set(1.0_r / (*out_filmArea * cosTheta * cosTheta * cosTheta * cosTheta));
	*out_pdfW = 1.0_r / (*out_filmArea * cosTheta * cosTheta * cosTheta);*/
}



// command interface

PinholeCamera::PinholeCamera(const InputPacket& packet) :
	PerspectiveCamera(packet)
{

}

SdlTypeInfo PinholeCamera::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_CAMERA, "pinhole");
}

ExitStatus PinholeCamera::ciExecute(const std::shared_ptr<PinholeCamera>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph