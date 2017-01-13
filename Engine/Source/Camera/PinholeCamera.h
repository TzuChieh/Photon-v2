#pragma once

#include "Camera/Camera.h"
#include "Common/primitive_type.h"

namespace ph
{

class PinholeCamera final : public Camera
{
public:
	PinholeCamera();
	virtual ~PinholeCamera() override;

	virtual void genSensingRay(const Sample& sample, Ray* const out_ray, const float32 aspectRatio) const override;
	virtual void evalEmittedImportance(const Vector3f& targetPos, Vector3f* const out_importance) const override;

private:
	float32 m_fov;
};

}// end namespace ph