//#include "Core/Estimator/SurfaceAttributeEstimator.h"
//#include "Core/Ray.h"
//#include "Core/Estimator/Integrand.h"
//#include "Core/Estimator/Estimation.h"
//#include "Core/SurfaceHit.h"
//#include "Core/HitProbe.h"
//#include "World/Scene.h"
//#include "Common/assertion.h"
//
//#include <limits>
//
//namespace ph
//{
//
//SurfaceAttributeEstimator::SurfaceAttributeEstimator() = default;
//
//AttributeTags SurfaceAttributeEstimator::supportedAttributes() const
//{
//	AttributeTags supports;
//	supports.tag(EAttribute::NORMAL);
//	return supports;
//}
//
//void SurfaceAttributeEstimator::update(const Scene& scene)
//{}
//
//void SurfaceAttributeEstimator::estimate(
//	const Ray&           ray,
//	const Integrand&     integrand,
//	const AttributeTags& requestedAttributes,
//	Estimation&          out_estimation) const
//{
//	Ray probingRay = Ray(ray).reverse();
//	probingRay.setMaxT(std::numeric_limits<real>::max());
//	
//	HitProbe probe;
//	if(integrand.getScene().isIntersecting(probingRay, &probe))
//	{
//		SurfaceHit surface(probingRay, probe);
//		estimate(surface, requestedAttributes, out_estimation);
//	}
//	else
//	{
//		out_estimation.set<EAttribute::NORMAL>(Vector3R(0));
//	}
//}
//
//void SurfaceAttributeEstimator::estimate(
//	const SurfaceHit&    surface,
//	const AttributeTags& requestedAttributes,
//	Estimation&          out_estimation) const
//{
//	//PH_ASSERT(surface.getDetail().getPrimitive());
//	// FIXME: SurfaceHit can be invalid (hit nothing), how to handle it?
//	if(!surface.getDetail().getPrimitive())
//	{
//		return;
//	}
//
//	if(requestedAttributes.isTagged(EAttribute::NORMAL))
//	{
//		out_estimation.set<EAttribute::NORMAL>(surface.getShadingNormal());
//	}
//}
//
//// command interface
//
//SurfaceAttributeEstimator::SurfaceAttributeEstimator(const InputPacket& packet) :
//	Estimator(packet)
//{}
//
//SdlTypeInfo SurfaceAttributeEstimator::ciTypeInfo()
//{
//	return SdlTypeInfo(ETypeCategory::REF_ESTIMATOR, "surface-attribute");
//}
//
//void SurfaceAttributeEstimator::ciRegister(CommandRegister& cmdRegister)
//{}
//
//}// end namespace ph