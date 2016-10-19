#include "Camera/Camera.h"

namespace ph
{
	
Camera::Camera() : 
	m_position(0, 0, 0), m_direction(0, 0, -1)
{

}

Camera::~Camera() = default;

}// end namespace ph