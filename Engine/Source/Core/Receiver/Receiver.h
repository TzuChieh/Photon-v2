#pragma once

#include "Common/primitive_type.h"
#include "Math/TVector2.h"
#include "Math/TVector3.h"
#include "Common/assertion.h"
#include "Core/Quantity/Spectrum.h"

namespace ph::math { class RigidTransform; }

namespace ph
{

class Ray;
class RayDifferential;

// TODO: time
class Receiver
{
public:
	explicit Receiver(const math::RigidTransform* receiverToWorld);

	inline virtual ~Receiver() = default;

	/*! @brief Generate a ray received by the receiver.

	Given raster coordinates, generate a corresponding ray that would have
	hit that location from the light entry of the receiver. The light entry
	would be the outmost surface of a camera's lens system for example. This
	method is for Monte-Carlo sampling.

	@return A weighting factor for the received quantity. Proper sensor response
	and sampling PDF are included in the returned weight.
	*/
	virtual Spectrum receiveRay(const math::Vector2D& rasterCoord, Ray* out_ray) const = 0;

	// Given a ray generated by receiveRay() along with the parameters for it, 
	// calculates differential information on the origin of the ray.
	// The default implementation uses numerical differentiation for 
	// the differentials.
	/*virtual void calcSensedRayDifferentials(const math::Vector2R& filmNdcPos, const Ray& sensedRay,
	                                        RayDifferential* out_result) const;*/

	virtual void evalEmittedImportanceAndPdfW(const math::Vector3R& targetPos, math::Vector2R* const out_filmCoord, math::Vector3R* const out_importance, real* out_filmArea, real* const out_pdfW) const = 0;

	const math::RigidTransform& getReceiverToWorld() const;

private:
	const math::RigidTransform* m_receiverToWorld;
};

// In-header Implementations:

inline const math::RigidTransform& Receiver::getReceiverToWorld() const
{
	PH_ASSERT(m_receiverToWorld);

	return *m_receiverToWorld;
}

}// end namespace ph
