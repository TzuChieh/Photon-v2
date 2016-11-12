#include "Camera/Camera.h"

namespace ph
{
	
Camera::Camera(const uint32 filmWidthPx, const uint32 filmHeightPx) :
	m_position(0, 0, 0), m_direction(0, 0, -1), m_film(filmWidthPx, filmHeightPx)
{

}

Camera::~Camera() = default;

}// end namespace ph