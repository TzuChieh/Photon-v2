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

	virtual void genSensingRay(const Sample& sample, Ray* const out_ray) const override;
	virtual void evalEmittedImportanceAndPdfW(const Vector3f& targetPos, Vector2f* const out_filmCoord, Vector3f* const out_importance, float32* const out_pdfW) const override;

private:
	float32 m_fov;
};

}// end namespace ph