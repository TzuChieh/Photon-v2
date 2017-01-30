#include "Camera/Camera.h"
#include "FileIO/InputPacket.h"

namespace ph
{
	
Camera::Camera() :
	m_position(0, 0, 0), m_direction(0, 0, -1), m_film(nullptr)
{

}

Camera::Camera(const InputPacket& packet) : 
	Camera()
{
	m_position = packet.getVector3R("position", m_position, "Camera >> argument position not found");
	m_direction = packet.getVector3R("direction", m_direction, "Camera >> argument direction not found");

	m_direction.normalizeLocal();
}

Camera::~Camera() = default;

}// end namespace ph