#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/BSDF.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>

namespace ph
{

class IdealSubstance : public Material, public TCommandInterface<IdealSubstance>
{
public:
	IdealSubstance();
	virtual ~IdealSubstance() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* out_surfaceBehavior) const override;

	void setDielectricReflector(real iorOuter, real iorInner);
	void setMetallicReflector(real iorOuter, const SpectralStrength& f0);

private:
	enum class Type
	{
		DIELECTRIC_REFLECTOR,
		METALLIC_REFLECTOR
	};

	Type m_type;

	real             m_monoIorOuter;
	real             m_monoIorInner;
	SpectralStrength m_f0;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<IdealSubstance> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<IdealSubstance>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph