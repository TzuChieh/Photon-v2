#include "Camera/PinholeCamera.h"
#include "Core/Ray.h"
#include "Core/Sample.h"
#include "Filmic/Film.h"
#include "FileIO/InputPacket.h"
#include "Math/Math.h"

#include <limits>

namespace ph
{

PinholeCamera::PinholeCamera() :
	Camera(),
	m_fov(50.0f / 180.0f * 3.1415927f)
{

}

PinholeCamera::PinholeCamera(const InputPacket& packet) : 
	Camera(packet)
{
	const real fovDegree = packet.getReal("fov-degree", 50, "at PinholeCamera()");
	m_fov = Math::toRadians(fovDegree);
}

PinholeCamera::~PinholeCamera() = default;

void PinholeCamera::genSensingRay(const Sample& sample, Ray* const out_ray) const
{
	const float32 aspectRatio = static_cast<float32>(getFilm()->getWidthPx()) / static_cast<float32>(getFilm()->getHeightPx());

	// Note: this will fail when the camera is facing directly on y-axis

	Vector3f rightDir = Vector3f(-getDirection().z, 0.0f, getDirection().x).normalizeLocal();
	Vector3f upDir = rightDir.cross(getDirection()).normalizeLocal();

	const float32 halfWidth = std::tan(m_fov / 2.0f);
	const float32 halfHeight = halfWidth / aspectRatio;

	const float32 pixelPosX = sample.m_cameraX * halfWidth;
	const float32 pixelPosY = sample.m_cameraY * halfHeight;

	rightDir.mulLocal(pixelPosX);
	upDir.mulLocal(pixelPosY);

	out_ray->setDirection(getDirection().add(rightDir.addLocal(upDir)).mulLocal(-1.0f).normalizeLocal());
	out_ray->setOrigin(getPosition());
	out_ray->setMinT(RAY_T_EPSILON);
	out_ray->setMaxT(RAY_T_MAX);
}

void PinholeCamera::evalEmittedImportanceAndPdfW(const Vector3f& targetPos, Vector2f* const out_filmCoord, Vector3f* const out_importance, float32* out_filmArea, float32* const out_pdfW) const
{
	const Vector3f targetDir = targetPos.sub(getPosition()).normalizeLocal();
	const float32 cosTheta = targetDir.dot(getDirection());

	if(cosTheta <= 0.0f)
	{
		out_importance->set(0.0f);
		*out_pdfW = 0.0f;
		return;
	}

	const Vector3f filmPos = targetDir.mul(1.0f / cosTheta).add(getPosition());
	const Vector3f filmCenter = getDirection().add(getPosition());
	const Vector3f filmVec = filmPos.sub(filmCenter);

	// Note: this will fail when the camera is facing directly on y-axis

	const Vector3f rightDir = Vector3f(-getDirection().z, 0.0f, getDirection().x).normalizeLocal();
	const Vector3f upDir = rightDir.cross(getDirection()).normalizeLocal();

	const float32 aspectRatio = static_cast<float32>(getFilm()->getWidthPx()) / static_cast<float32>(getFilm()->getHeightPx());
	const float32 halfFilmWidth = std::tan(m_fov / 2.0f);
	const float32 halfFilmHeight = halfFilmWidth / aspectRatio;

	out_filmCoord->x = filmVec.dot(rightDir) / halfFilmWidth;
	out_filmCoord->y = filmVec.dot(upDir) / halfFilmHeight;

	if(std::abs(out_filmCoord->x) > 1.0f || std::abs(out_filmCoord->y) > 1.0f)
	{
		out_importance->set(0.0f);
		*out_pdfW = 0.0f;
		return;
	}

	*out_filmArea = halfFilmWidth * halfFilmHeight * 4.0f;
	out_importance->set(1.0f / (*out_filmArea * cosTheta * cosTheta * cosTheta * cosTheta));
	*out_pdfW = 1.0f / (*out_filmArea * cosTheta * cosTheta * cosTheta);
}

}// end namespace ph