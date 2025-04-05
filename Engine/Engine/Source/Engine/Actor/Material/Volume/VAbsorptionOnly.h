#pragma once

#include "Engine/Actor/Material/VolumeMaterial.h"
#include "Engine/Math/Color/Spectrum.h"
#include "Engine/Math/math_fwd.h"

#include <Common/primitive_type.h>

namespace ph
{

class VAbsorptionOnly final : public VolumeMaterial
{
public:
	VAbsorptionOnly();
	explicit VAbsorptionOnly(const math::Vector3R& absorptionCoeffSrgb);
	explicit VAbsorptionOnly(const math::Spectrum& absorptionCoeff);

	void genVolume(const CookingContext& ctx, VolumeBehavior& behavior) const override;

private:
	math::Spectrum m_absorptionCoeff;
};

}// end namespace ph
