#include "Core/Receiver/PinholeCamera.h"
#include "Core/Ray.h"
#include "Math/math.h"
#include "Math/Random.h"
#include "Common/assertion.h"
#include "Math/Transform/Transform.h"
#include "Math/Transform/RigidTransform.h"

#include <limits>

namespace ph
{

PinholeCamera::PinholeCamera(
	const math::Vector2D&             sensorSize,
	const math::Transform* const      rasterToSensor,
	const math::RigidTransform* const receiverToWorld) : 

	RectangularSensorReceiver(
		sensorSize,
		rasterToSensor,
		receiverToWorld)
{}

Spectrum PinholeCamera::receiveRay(const math::Vector2D& rasterCoord, Ray* const out_ray) const
{
	PH_ASSERT(out_ray);

	// TODO: time info

	math::Vector3R pinholePos;
	getCameraToWorld().transformP({0, 0, 0}, &pinholePos);

	out_ray->setDirection(genReceiveRayDir(rasterCoord));
	out_ray->setOrigin(pinholePos);
	out_ray->setMinT(0.0001_r);// HACK: hard-coded number
	out_ray->setMaxT(std::numeric_limits<real>::max());

	// HACK
	Time time;
	time.relativeT = math::Random::genUniformReal_i0_e1();
	out_ray->setTime(time);

	PH_ASSERT_MSG(out_ray->getOrigin().isFinite() && out_ray->getDirection().isFinite(), "\n"
		"origin    = " + out_ray->getOrigin().toString() + "\n"
		"direction = " + out_ray->getDirection().toString() + "\n");

	return Spectrum(1);
}

math::Vector3R PinholeCamera::genReceiveRayDir(const math::Vector2D& rasterCoord) const
{
	// Direction vector is computed in camera space then transformed to world
	// space for better numerical precision. Subtracting two world space 
	// points can lead to high numerical error when camera is far from origin
	// (edge aliasing-like artifacts can be observed ~100 meters away from origin).

	math::Vector3R sensorPos;
	getRasterToSensor().transformP(
		math::Vector3R(math::Vector3D(rasterCoord.x(), rasterCoord.y(), 0)),
		&sensorPos);

	// Subtracting pinhole position is omitted since it is at (0, 0, 0)
	math::Vector3R sensedRayDir;
	getCameraToWorld().transformV(sensorPos, &sensedRayDir);

	return sensedRayDir.normalize();
}

void PinholeCamera::evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const
{
	PH_ASSERT_UNREACHABLE_SECTION();

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

}// end namespace ph
