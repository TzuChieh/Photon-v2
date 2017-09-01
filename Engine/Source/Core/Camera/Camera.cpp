#include "Core/Camera/Camera.h"
#include "FileIO/InputPacket.h"

namespace ph
{

Camera::Camera() :
	Camera(Vector3R(0, 0, 0), Vector3R(0, 0, -1), Vector3R(0, 1, 0))
{

}

Camera::Camera(const Vector3R& position, const Vector3R& direction, const Vector3R& upAxis) : 
	m_position(position), m_direction(direction.normalize()), m_upAxis(upAxis.normalize()),
	m_film(nullptr)
{
	// TODO: input maybe invalid (e.g., axes to close to each other)

	updateCameraToWorldTransform(m_position, m_direction, m_upAxis);
}

Camera::~Camera() = default;

void Camera::onFilmSet(Film* newFilm)
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