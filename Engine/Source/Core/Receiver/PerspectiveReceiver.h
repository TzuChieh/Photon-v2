#pragma once

#include "Core/Receiver/Receiver.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "Math/Transform/TDecomposedTransform.h"

#include <iostream>

namespace ph::math { class Transform; }

namespace ph
{

class Ray;

class PerspectiveReceiver : public Receiver
{
public:
	// TODO: ordinary ctors

	Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const override = 0;
	void evalEmittedImportanceAndPdfW(
		const math::Vector3R& targetPos,
		math::Vector2R* const out_filmCoord,
		math::Vector3R* const out_importance,
		real* out_filmArea, 
		real* const out_pdfW) const override = 0;

protected:
	std::shared_ptr<math::Transform>    m_receiverToWorld;// FIXME: should be rigid
	std::shared_ptr<math::Transform>    m_rasterToReceiver;
	math::TDecomposedTransform<float64> m_rasterToReceiverDecomposed;
	
private:
	float64 m_sensorWidth;
	float64 m_sensorOffset;

	void updateTransforms();
};

}// end namespace ph
