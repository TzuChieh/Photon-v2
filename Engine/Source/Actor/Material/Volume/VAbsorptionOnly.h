#pragma once

#include "Actor/Material/VolumeMaterial.h"
#include "Core/Quantity/Spectrum.h"
#include "Math/math_fwd.h"
#include "Common/primitive_type.h"

namespace ph
{

class VAbsorptionOnly final : public VolumeMaterial
{
public:
	VAbsorptionOnly();
	explicit VAbsorptionOnly(const math::Vector3R& absorptionCoeffSrgb);
	explicit VAbsorptionOnly(const Spectrum& absorptionCoeff);

	void genVolume(CookingContext& context, VolumeBehavior& behavior) const override;

private:
	Spectrum m_absorptionCoeff;
};

}// end namespace ph
