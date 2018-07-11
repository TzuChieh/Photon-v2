#pragma once

#include "Core/Estimator/Estimator.h"

namespace ph
{

class SurfaceAttributeEstimator : public Estimator, public TCommandInterface<SurfaceAttributeEstimator>
{
public:
	~SurfaceAttributeEstimator() override;

	void update(const Scene& scene) override;
	void radianceAlongRay(const Ray& ray, const Integrand& integrand, std::vector<SenseEvent>& out_senseEvents) const override;

// command interface
public:
	explicit SurfaceAttributeEstimator(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph