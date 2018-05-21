#pragma once

#include "Actor/Material/VolumeMaterial.h"
#include "Core/Quantity/SpectralStrength.h"
#include "Math/math_fwd.h"
#include "Common/primitive_type.h"

namespace ph
{

class VAbsorptionOnly final : public VolumeMaterial
{
public:
	VAbsorptionOnly();
	VAbsorptionOnly(const Vector3R& absorptionCoeffSrgb);
	VAbsorptionOnly(const SpectralStrength& absorptionCoeff);

	void genVolume(CookingContext& context, VolumeBehavior& behavior) const override;

private:
	SpectralStrength m_absorptionCoeff;

// command interface
public:
	explicit VAbsorptionOnly(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph