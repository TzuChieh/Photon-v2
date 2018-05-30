#include "Core/Camera/Camera.h"
#include "FileIO/SDL/InputPacket.h"
#include "Core/RayDifferential.h"
#include "Core/Ray.h"

namespace ph
{

Camera::Camera() :
	Camera(Vector3R(0, 0, 0), Vector3R(0, 0, -1), Vector3R(0, 1, 0))
{}

Camera::Camera(const Vector3R& position, const Vector3R& direction, const Vector3R& upAxis) : 
	m_position(position), m_direction(direction.normalize()), m_upAxis(upAxis.normalize()),
	m_film(nullptr)
{
	// TODO: input maybe invalid (e.g., axes to close to each other)

	updateCameraToWorldTransform(m_position, m_direction, m_upAxis);
}

void Camera::calcSensedRayDifferentials(
	const Vector2R& rasterPosPx, const Ray& sensedRay,
	RayDifferential* const out_result) const
{
	// 2nd-order accurate with respect to the size of <deltaPx>
	const real deltaPx        = 1.0_r / 32.0_r;
	const real reciIntervalPx = 1.0_r / deltaPx;

	Ray dnxRay, dpxRay, dnyRay, dpyRay;
	genSensedRay(Vector2R(rasterPosPx.x - deltaPx, rasterPosPx.y), &dnxRay);
	genSensedRay(Vector2R(rasterPosPx.x + deltaPx, rasterPosPx.y), &dpxRay);
	genSensedRay(Vector2R(rasterPosPx.x, rasterPosPx.y - deltaPx), &dnyRay);
	genSensedRay(Vector2R(rasterPosPx.x, rasterPosPx.y + deltaPx), &dpyRay);

	out_result->setPartialPs((dpxRay.getOrigin() - dnxRay.getOrigin()).divLocal(reciIntervalPx),
	                         (dpyRay.getOrigin() - dnyRay.getOrigin()).divLocal(reciIntervalPx));

	out_result->setPartialDs((dpxRay.getDirection() - dnxRay.getDirection()).divLocal(reciIntervalPx),
	                         (dpyRay.getDirection() - dnyRay.getDirection()).divLocal(reciIntervalPx));
}

Camera::~Camera() = default;

void Camera::onFilmSet(SpectralSamplingFilm* /* newFilm */)
{
	// do nothing
}

void Camera::updateCameraToWorldTransform(const Vector3R& position, const Vector3R& direction, const Vector3R& upAxis)
{
	m_cameraToWorldTransform.setPosition(TVector3<hiReal>(position));

	// changing unit from mm to m
	m_cameraToWorldTransform.setScale(TVector3<hiReal>(0.001, 0.001, 0.001));

	const TVector3<hiReal> zAxis             = TVector3<hiReal>(direction.mul(-1.0f)).normalizeLocal();
	const TVector3<hiReal> xAxis             = TVector3<hiReal>(direction.cross(upAxis)).normalizeLocal();
	const TVector3<hiReal> yAxis             = zAxis.cross(xAxis).normalizeLocal();
	const TMatrix4<hiReal> worldToViewRotMat = TMatrix4<hiReal>().initRotation(xAxis, yAxis, zAxis);
	m_cameraToWorldTransform.setRotation(TQuaternion<hiReal>(worldToViewRotMat).conjugateLocal());
}

// command interface

Camera::Camera(const InputPacket& packet) :
	Camera()
{
	// TODO: input maybe invalid (e.g., axes to close to each other)

	m_position  = packet.getVector3r("position",  m_position,  DataTreatment::REQUIRED());
	m_direction = packet.getVector3r("direction", m_direction, DataTreatment::REQUIRED());
	m_upAxis    = packet.getVector3r("up-axis",   m_upAxis,    DataTreatment::OPTIONAL());

	m_direction.normalizeLocal();
	m_upAxis.normalizeLocal();

	updateCameraToWorldTransform(m_position, m_direction, m_upAxis);
}

SdlTypeInfo Camera::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_CAMERA, "camera");
}

void Camera::ciRegister(CommandRegister& cmdRegister) {}

}// end namespace ph