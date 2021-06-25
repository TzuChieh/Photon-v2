#pragma once

#include "Core/Receiver/PerspectiveReceiver.h"

namespace ph
{

class PinholeCamera : public PerspectiveReceiver
{
public:
	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;
	void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const override;

private:
	const math::Vector3R& getPinholePos() const;
	math::Vector3R genReceiveRayDir(const math::Vector2D& rasterCoord) const;
};

// In-header Implementations:

inline const math::Vector3R& PinholeCamera::getPinholePos() const
{
	return getPosition();
}

}// end namespace ph
