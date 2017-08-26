#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/BSDF.h"

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

private:
	enum class Type
	{
		DIELECTRIC_REFLECTOR
	};

	Type m_type;

	real m_monoIorOuter;
	real m_monoIorInner;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static std::unique_ptr<IdealSubstance> ciLoad(const InputPacket& packet);
	static ExitStatus ciExecute(const std::shared_ptr<IdealSubstance>& targetResource, 
	                            const std::string& functionName, 
	                            const InputPacket& packet);
};

}// end namespace ph