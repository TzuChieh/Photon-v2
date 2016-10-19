#pragma once

#include "Camera/Camera.h"

namespace ph
{

class DefaultCamera final : public Camera
{
public:
	DefaultCamera();
	virtual ~DefaultCamera() override;

	virtual void genSampleRay(Ray* const out_ray, const uint32 xRes, const uint32 yRes, float32 x, float32 y) const override;
};

}// end namespace ph