#pragma once

#include "Core/Receiver/Receiver.h"
#include "Common/primitive_type.h"
#include "Math/TVector2.h"

#include <memory>

namespace ph::math { class RigidTransform; }

namespace ph
{

class RadiantFluxPanel : public Receiver
{
public:
	RadiantFluxPanel(
		const math::Vector2R& widthHeight,
		const math::Vector3R& position,
		const math::Vector3R& direction,
		const math::Vector3R& upAxis);

	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;
	void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	math::Vector2R                        m_widthHeight;
	std::shared_ptr<math::RigidTransform> m_receiverToWorld;
};

}// end namespace ph
