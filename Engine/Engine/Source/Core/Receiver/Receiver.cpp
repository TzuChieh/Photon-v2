#include "Core/Receiver/Receiver.h"
#include "Core/RayDifferential.h"
#include "Core/Ray.h"

#include <Common/logging.h>

#include <string>

namespace ph
{

PH_DEFINE_INTERNAL_LOG_GROUP(Receiver, Receiver);

Receiver::Receiver(const math::RigidTransform* const receiverToWorld) : 
	m_receiverToWorld(receiverToWorld)
{
	PH_ASSERT(m_receiverToWorld);
}

//void Camera::calcSensedRayDifferentials(
//	const math::Vector2R& rasterPosPx, const Ray& sensedRay,
//	RayDifferential* const out_result) const
//{
//	// 2nd-order accurate with respect to the size of <deltaPx>
//	const real deltaPx        = 1.0_r / 32.0_r;
//	const real reciIntervalPx = 1.0_r / deltaPx;
//
//	Ray dnxRay, dpxRay, dnyRay, dpyRay;
//	receiveRay(math::Vector2R(rasterPosPx.x - deltaPx, rasterPosPx.y), &dnxRay);
//	receiveRay(math::Vector2R(rasterPosPx.x + deltaPx, rasterPosPx.y), &dpxRay);
//	receiveRay(math::Vector2R(rasterPosPx.x, rasterPosPx.y - deltaPx), &dnyRay);
//	receiveRay(math::Vector2R(rasterPosPx.x, rasterPosPx.y + deltaPx), &dpyRay);
//
//	out_result->setPartialPs((dpxRay.getOrigin() - dnxRay.getOrigin()).divLocal(reciIntervalPx),
//	                         (dpyRay.getOrigin() - dnyRay.getOrigin()).divLocal(reciIntervalPx));
//
//	out_result->setPartialDs((dpxRay.getDirection() - dnxRay.getDirection()).divLocal(reciIntervalPx),
//	                         (dpyRay.getDirection() - dnyRay.getDirection()).divLocal(reciIntervalPx));
//}

}// end namespace ph
