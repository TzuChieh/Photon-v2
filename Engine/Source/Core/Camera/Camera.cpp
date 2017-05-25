#include "Core/Camera/Camera.h"
#include "FileIO/InputPacket.h"

namespace ph
{

Camera::Camera() :
	m_position(0, 0, 0), m_direction(0, 0, -1), m_upAxis(0, 1, 0), m_film(nullptr)
{

}

Camera::~Camera() = default;

void Camera::onFilmSet(Film* newFilm)
{
	// do nothing
}

void Camera::updateCameraToWorldTransform(const Vector3R& position, const Vector3R& direction, const Vector3R& upAxis)
{
	m_cameraToWorldTransform.setPosition(TVector3<hiReal>(position));
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

ExitStatus Camera::ciExecute(const std::shared_ptr<Camera>& targetResource, const std::string& functionName, const InputPacket& packet)
{
	return ExitStatus::UNSUPPORTED();
}

}// end namespace ph