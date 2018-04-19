#include "Core/Camera/PinholeCamera.h"
#include "Core/Ray.h"
#include "Core/Sample.h"
#include "Core/Filmic/Film.h"
#include "FileIO/InputPacket.h"
#include "Math/Math.h"
#include "Math/Random.h"
#include "Common/assertion.h"

#include <limits>

namespace ph
{

PinholeCamera::~PinholeCamera() = default;

void PinholeCamera::genSensedRay(const Vector2R& rasterPosPx, Ray* const out_ray) const
{
	out_ray->setDirection(genSensedRayDir(rasterPosPx));
	out_ray->setOrigin(getPinholePos());
	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
	out_ray->setMaxT(std::numeric_limits<real>::max());

	// HACK
	Time time;
	time.relativeT = Random::genUniformReal_i0_e1();
	out_ray->setTime(time);

	PH_ASSERT_MSG(out_ray->getOrigin().isFinite() && out_ray->getDirection().isFinite(), "\n"
		"origin    = " + out_ray->getOrigin().toString() + "\n"
		"direction = " + out_ray->getDirection().toString() + "\n");
}

Vector3R PinholeCamera::genSensedRayDir(const Vector2R& rasterPosPx) const
{
	Vector3R filmPos;
	m_rasterToWorld->transformP(Vector3R(rasterPosPx.x, rasterPosPx.y, 0), &filmPos);
	return filmPos.sub(getPinholePos()).normalizeLocal();
}

void PinholeCamera::evalEmittedImportanceAndPdfW(const Vector3R& targetPos, Vector2R* const out_filmCoord, Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const
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

void PinholeCamera::ciRegister(CommandRegister& cmdRegister)
{
	SdlLoader loader;
	loader.setFunc<PinholeCamera>(ciLoad);
	cmdRegister.setLoader(loader);
}

std::unique_ptr<PinholeCamera> PinholeCamera::ciLoad(const InputPacket& packet)
{
	return std::make_unique<PinholeCamera>(packet);
}

}// end namespace ph