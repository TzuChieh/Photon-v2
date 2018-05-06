#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"
#include "Core/SurfaceBehavior/Property/FresnelEffect.h"

#include <functional>
#include <memory>

namespace ph
{

class AbradedOpaque : public SurfaceMaterial, public TCommandInterface<AbradedOpaque>
{
public:
	AbradedOpaque();
	virtual ~AbradedOpaque() override;

private:
	virtual std::shared_ptr<SurfaceOptics> genSurfaceOptics(CookingContext& context) const override;

	std::function<std::unique_ptr<SurfaceOptics>()> m_opticsGenerator;

// command interface
public:
	AbradedOpaque(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::function<std::unique_ptr<SurfaceOptics>()> loadITR(const InputPacket& packet);
	static std::function<std::unique_ptr<SurfaceOptics>()> loadATR(const InputPacket& packet);
	static std::unique_ptr<FresnelEffect> loadFresnelEffect(const InputPacket& packet);
};

}// end namespace ph