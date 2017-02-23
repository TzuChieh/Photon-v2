#pragma once

#include "Core/Camera/Camera.h"
#include "Common/primitive_type.h"

namespace ph
{

class PinholeCamera final : public Camera
{
public:
	PinholeCamera();
	PinholeCamera(const InputPacket& packet);
	virtual ~PinholeCamera() override;

	virtual void genSensingRay(const Sample& sample, Ray* const out_ray) const override;
	virtual void evalEmittedImportanceAndPdfW(const Vector3R& targetPos, Vector2f* const out_filmCoord, Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	real m_fov;
};

}// end namespace ph