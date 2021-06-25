#include "Actor/Material/Volume/VAbsorptionOnly.h"
#include "Core/VolumeBehavior/VolumeBehavior.h"
#include "Core/VolumeBehavior/VolumeOptics/VoHomoAbsorption.h"
#include "Core/VolumeBehavior/BlockFunction/BfConstant.h"

namespace ph
{

VAbsorptionOnly::VAbsorptionOnly() : 
	VAbsorptionOnly(math::Vector3R(0.5_r))
{}

VAbsorptionOnly::VAbsorptionOnly(const math::Vector3R& absorptionCoeffSrgb) :
	VAbsorptionOnly(Spectrum().setLinearSrgb(absorptionCoeffSrgb, EQuantity::RAW))
{}

VAbsorptionOnly::VAbsorptionOnly(const Spectrum& absorptionCoeff) : 
	VolumeMaterial(),
	m_absorptionCoeff(absorptionCoeff)
{}

void VAbsorptionOnly::genVolume(CookingContext& context, VolumeBehavior& behavior) const
{
	auto blockFunc = std::make_shared<BfConstant>(m_absorptionCoeff);
	behavior.setOptics(std::make_shared<VoHomoAbsorption>(blockFunc));
}

}// end namespace ph
