#pragma once

//#include "Core/Estimator/Estimator.h"
//#include "Core/Quantity/SpectralStrength.h"
//#include "Core/Estimator/SurfaceAttributeEstimator.h"
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
//		SpectralStrength& out_radiance,
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
