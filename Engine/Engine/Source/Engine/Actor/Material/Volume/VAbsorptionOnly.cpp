#include "Engine/Actor/Material/Volume/VAbsorptionOnly.h"
#include "Engine/Core/VolumeBehavior/VolumeBehavior.h"
#include "Engine/Core/VolumeBehavior/VolumeOptics/HomogeneousAbsorption.h"
//#include "Engine/Core/VolumeBehavior/Property/MediumCoefficient.h"

namespace ph
{

VAbsorptionOnly::VAbsorptionOnly() : 
	VAbsorptionOnly(math::Vector3R(0.5_r))
{}

VAbsorptionOnly::VAbsorptionOnly(const math::Vector3R& absorptionCoeffSrgb) :
	VAbsorptionOnly(math::Spectrum().setLinearSRGB(absorptionCoeffSrgb.toArray(), math::EColorUsage::Raw))
{}

VAbsorptionOnly::VAbsorptionOnly(const math::Spectrum& absorptionCoeff) : 
	VolumeMaterial(),
	m_absorptionCoeff(absorptionCoeff)
{}

void VAbsorptionOnly::genVolume(const CookingContext& ctx, VolumeBehavior& behavior) const
{
	/*auto blockFunc = std::make_shared<BfConstant>(m_absorptionCoeff);
	behavior.setOptics(std::make_shared<HomogeneousAbsorption>(blockFunc));*/
}

}// end namespace ph
