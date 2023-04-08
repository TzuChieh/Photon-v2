#include "Designer/ViewportCamera.h"

#include <Math/math.h>

namespace ph::editor
{

ViewportCamera::ViewportCamera()
	: m_type(EType::Perspective)
	, m_transform()
	, m_fov(math::to_radians(70.0_r))
	, m_nearClippingDistance(0.1_r)
	, m_farClippingDistance(2000.0_r)
	, m_resolution(1280, 720)
{}

}// end namespace ph::editor
