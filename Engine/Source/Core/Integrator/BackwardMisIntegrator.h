#pragma once

#include "Common/primitive_type.h"
#include "Core/Integrator/Integrator.h"

namespace ph
{

class BackwardMisIntegrator final : public Integrator
{
public:
	virtual ~BackwardMisIntegrator() override;

	virtual void update(const World& world) override;
	virtual void radianceAlongRay(const Ray& ray, const World& world, Vector3f* const out_radiance) const override;

private:
	static void rationalClamp(Vector3f& value);
	static float32 misWeight(float32 pdf1W, float32 pdf2W);
};

}// end namespace ph