#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceOptics/OpaqueMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"

#include <functional>
#include <memory>

namespace ph
{

class AbradedOpaque : public Material, public TCommandInterface<AbradedOpaque>
{
public:
	AbradedOpaque();
	virtual ~AbradedOpaque() override;

	virtual void populateSurfaceBehavior(CookingContext& context, SurfaceBehavior* out_surfaceBehavior) const override;

private:
	std::function<std::unique_ptr<SurfaceOptics>()> m_opticsGenerator;

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<AbradedOpaque> ciLoad(const InputPacket& packet);
	static std::unique_ptr<AbradedOpaque> ciLoadITR(const InputPacket& packet);
	static std::unique_ptr<AbradedOpaque> ciLoadATR(const InputPacket& packet);
};

}// end namespace ph