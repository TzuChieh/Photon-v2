#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/BSDF.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>
#include <functional>

namespace ph
{

class IdealSubstance : public Material, public TCommandInterface<IdealSubstance>
{
public:
	IdealSubstance();
	virtual ~IdealSubstance() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* out_surfaceBehavior) const override;

private:
	std::function<std::unique_ptr<BSDF>()> m_bsdfGenerator;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<IdealSubstance> ciLoad(const InputPacket& packet);
};

}// end namespace ph