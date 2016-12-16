#pragma once

#include "Camera/Camera.h"
#include "Common/primitive_type.h"

namespace ph
{

class DefaultCamera final : public Camera
{
public:
	DefaultCamera();
	virtual ~DefaultCamera() override;

	virtual void genSensingRay(const Sample& sample, Ray* const out_ray, const float32 aspectRatio) const override;

private:
	float32 m_fov;
};

}// end namespace ph