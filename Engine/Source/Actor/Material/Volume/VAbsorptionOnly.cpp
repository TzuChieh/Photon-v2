#include "Actor/Material/Volume/VAbsorptionOnly.h"
#include "Core/VolumeBehavior/VolumeBehavior.h"
#include "Core/VolumeBehavior/VolumeOptics/VoHomoAbsorption.h"
#include "Core/VolumeBehavior/BlockFunction/BfConstant.h"
#include "FileIO/SDL/InputPacket.h"

namespace ph
{

VAbsorptionOnly::VAbsorptionOnly() : 
	VAbsorptionOnly(math::Vector3R(0.5_r))
{}

VAbsorptionOnly::VAbsorptionOnly(const math::Vector3R& absorptionCoeffSrgb) :
	VAbsorptionOnly(SpectralStrength().setLinearSrgb(absorptionCoeffSrgb, EQuantity::RAW))
{}

VAbsorptionOnly::VAbsorptionOnly(const SpectralStrength& absorptionCoeff) : 
	VolumeMaterial(),
	m_absorptionCoeff(absorptionCoeff)
{}

void VAbsorptionOnly::genVolume(CookingContext& context, VolumeBehavior& behavior) const
{
	auto blockFunc = std::make_shared<BfConstant>(m_absorptionCoeff);
	behavior.setOptics(std::make_shared<VoHomoAbsorption>(blockFunc));
}

// command interface

VAbsorptionOnly::VAbsorptionOnly(const InputPacket& packet) : 
	VolumeMaterial(packet),
	m_absorptionCoeff(0.5_r)
{
	if(packet.hasVector3("coeff"))
	{
		m_absorptionCoeff.setLinearSrgb(packet.getVector3("coeff"), EQuantity::RAW);
	}
	else
	{
		m_absorptionCoeff.setValues(packet.getReal("coeff", 0.5_r, DataTreatment::REQUIRED()));
	}
}

SdlTypeInfo VAbsorptionOnly::ciTypeInfo()
{
	return SdlTypeInfo(ETypeCategory::REF_MATERIAL, "volume-absorption");
}

void VAbsorptionOnly::ciRegister(CommandRegister& cmdRegister)
{
	cmdRegister.setLoader(SdlLoader([](const InputPacket& packet)
	{
		return std::make_unique<VAbsorptionOnly>(packet);
	}));
}

}// end namespace ph
