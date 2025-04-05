#pragma once

//#include "Engine/Core/Estimator/Estimator.h"
//#include "Engine/Core/Quantity/Spectrum.h"
//#include "Engine/Core/Estimator/SurfaceAttributeEstimator.h"
//
//namespace ph
//{
//
//class SurfaceHit;
//
//class PathEstimator : public Estimator, public TCommandInterface<PathEstimator>
//{
//public:
//	PathEstimator();
//
//	virtual void radianceAlongRay(
//		const Ray&        ray, 
//		const Integrand&  integrand, 
//		Spectrum& out_radiance,
//		SurfaceHit&       out_firstHit) const = 0;
//
//	AttributeTags supportedAttributes() const override;
//	void update(const Scene& scene) override;
//	void estimate(
//		const Ray&           ray,
//		const Integrand&     integrand,
//		const AttributeTags& requestedAttributes,
//		Estimation&          out_estimation) const override;
//
//private:
//	SurfaceAttributeEstimator m_surfaceAttributeEstimator;
//
//// command interface
//public:
//	explicit PathEstimator(const InputPacket& packet);
//	static SdlTypeInfo ciTypeInfo();
//	static void ciRegister(CommandRegister& cmdRegister);
//};
//
//}// end namespace ph
