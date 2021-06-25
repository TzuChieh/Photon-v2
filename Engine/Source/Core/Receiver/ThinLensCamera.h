#pragma once

#include "Core/Receiver/PerspectiveReceiver.h"

namespace ph
{

class ThinLensCamera : public PerspectiveReceiver
{
public:
	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override;
	void evalEmittedImportanceAndPdfW(
		const math::Vector3R& targetPos,
		math::Vector2R* const out_filmCoord,
		math::Vector3R* const out_importance,
		real* out_filmArea, 
		real* const out_pdfW) const override;

private:
	real m_lensRadiusMM;
	real m_focalDistanceMM;

	static void genRandomSampleOnDisk(const real radius, real* const out_x, real* const out_y);
};

}// end namespace ph
