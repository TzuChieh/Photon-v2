#pragma once

#include <Common/primitive_type.h>
#include <Math/Transform/TDecomposedTransform.h>
#include <Math/TVector2.h>

namespace ph::editor
{

class ViewportCamera final
{
public:
	enum EType
	{
		Perspective,
		Orthographic
	};

	ViewportCamera();

private:
	EType m_type;
	math::TDecomposedTransform<real> m_transform;
	real m_fov;
	real m_nearClippingDistance;
	real m_farClippingDistance;
	math::Vector2S m_resolution;
};

}// end namespace ph::editor
