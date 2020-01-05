#include "Core/Camera/PinholeCamera.h"
#include "Core/Ray.h"
#include "Core/Sample.h"
#include "Core/Filmic/TSamplingFilm.h"
#include "DataIO/SDL/InputPacket.h"
#include "Math/math.h"
#include "Math/Random.h"
#include "Common/assertion.h"

#include <limits>

namespace ph
{

void PinholeCamera::genSensedRay(const math::Vector2R& filmNdcPos, Ray* const out_ray) const
{
	PH_ASSERT(out_ray);
	out_ray->setDirection(genSensedRayDir(filmNdcPos));
	out_ray->setOrigin(getPinholePos());
	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
	out_ray->setMaxT(std::numeric_limits<real>::max());

	// HACK
	Time time;
	time.relativeT = math::Random::genUniformReal_i0_e1();
	out_ray->setTime(time);

	PH_ASSERT_MSG(out_ray->getOrigin().isFinite() && out_ray->getDirection().isFinite(), "\n"
		"origin    = " + out_ray->getOrigin().toString() + "\n"
		"direction = " + out_ray->getDirection().toString() + "\n");
}

math::Vector3R PinholeCamera::genSensedRayDir(const math::Vector2R& filmNdcPos) const
{
	// Direction vector is computed in camera space then transformed to world
	// space for better numerical precision. Subtracting two world space 
	// points can lead to high numerical error when camera is far from origin
	// (edge aliasing-like artifacts can be observed ~100 meters away from origin).

	math::Vector3R filmPosMM;
	m_filmToCamera->transformP(math::Vector3R(filmNdcPos.x, filmNdcPos.y, 0), &filmPosMM);

	// subtracting pinhole position is omitted since it is at (0, 0, 0) mm
	math::Vector3R sensedRayDir;
	m_cameraToWorld->transformV(filmPosMM, &sensedRayDir);

	return sensedRayDir.normalize();
}

void PinholeCamera::evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const
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
{}

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
