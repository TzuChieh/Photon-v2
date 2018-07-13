#pragma once

#include "Core/Estimator/Estimator.h"

namespace ph
{

class SurfaceHit;

class SurfaceAttributeEstimator : public Estimator, public TCommandInterface<SurfaceAttributeEstimator>
{
public:
	SurfaceAttributeEstimator();
	~SurfaceAttributeEstimator() override;

	AttributeTags supportedAttributes() const override;

	void update(const Scene& scene) override;

	void estimate(
		const Ray&           ray,
		const Integrand&     integrand,
		const AttributeTags& requestedAttributes,
		Estimation&          out_estimation) const override;

	void estimate(
		const SurfaceHit&    surface, 
		const AttributeTags& requestedAttributes, 
		Estimation&          out_estimation) const;

// command interface
public:
	explicit SurfaceAttributeEstimator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph