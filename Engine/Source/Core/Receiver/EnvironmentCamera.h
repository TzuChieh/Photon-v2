#pragma once

#include "Core/Receiver/Receiver.h"
#include "Math/Transform/StaticRigidTransform.h"

namespace ph
{

class EnvironmentCamera : public Receiver
{
public:
	EnvironmentCamera();

	EnvironmentCamera(
		const math::Vector3R&    position,
		const math::QuaternionR& rotation,
		const math::Vector2S&    resolution);

	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;

	void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	math::StaticRigidTransform m_receiverToWorld;
};

}// end namespace ph
