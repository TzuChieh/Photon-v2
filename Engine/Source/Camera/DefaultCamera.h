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

	virtual void genSampleRay(Ray* const out_ray, const uint32 xResPx, const uint32 yResPx, float32 xPx, float32 yPx) const override;

private:
	float32 m_fov;
};

}// end namespace ph