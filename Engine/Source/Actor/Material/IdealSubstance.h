#pragma once

#include "Common/primitive_type.h"
#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics.h"
#include "Core/Quantity/SpectralStrength.h"

#include <memory>
#include <functional>

namespace ph
{

class IdealSubstance : public SurfaceMaterial, public TCommandInterface<IdealSubstance>
{
public:
	IdealSubstance();
	~IdealSubstance() override;

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

	void asDielectricReflector(real iorInner, real iorOuter);

	// FIXME: specifying ior-outer is redundent, f0 already includes this
	void asMetallicReflector(const Vector3R& linearSrgbF0, real iorOuter);

	void asTransmitter(real iorInner, real iorOuter);
	void asAbsorber();

	void asDielectric(
		real iorInner, 
		real iorOuter, 
		const Vector3R& linearSrgbReflectionScale, 
		const Vector3R& linearSrgbTransmissionScale);

private:
	std::function<std::unique_ptr<SurfaceOptics>(CookingContext& context)> m_opticsGenerator;

// command interface
public:
	explicit IdealSubstance(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph