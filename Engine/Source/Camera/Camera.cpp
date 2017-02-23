#include "Camera/Camera.h"
#include "FileIO/InputPacket.h"

namespace ph
{
	
Camera::Camera() :
	m_position(0, 0, 0), m_direction(0, 0, -1), m_upAxis(0, 1, 0), m_film(nullptr)
{

}

Camera::Camera(const InputPacket& packet) : 
	Camera()
{
	const Vector3R position  = packet.getVector3r("position",  m_position,  DataTreatment::REQUIRED());
	const Vector3R direction = packet.getVector3r("direction", m_direction, DataTreatment::REQUIRED());
	const Vector3R upAxis    = packet.getVector3r("up-axis",   m_upAxis,    DataTreatment::OPTIONAL());

	setPosition(position);
	setDirection(direction);
	setUpAxis(upAxis);
}

Camera::~Camera() = default;

}// end namespace ph