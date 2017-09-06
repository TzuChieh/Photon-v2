#pragma once

#include "Actor/Material/Material.h"
#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class AbradedTranslucent : public Material, public TCommandInterface<AbradedTranslucent>
{
public:
	AbradedTranslucent();
	virtual ~AbradedTranslucent() override;

	virtual void populateSurfaceBehavior(SurfaceBehavior* out_surfaceBehavior) const override;

	//void setAlbedo(const Vector3R& albedo);
	void setIor(const real iorOuter, const real iorInner);
	void setRoughness(const real roughness);

private:
	TranslucentMicrofacet m_optics;

	static real roughnessToAlpha(const real roughness);

// command interface
public:
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
	static std::unique_ptr<AbradedTranslucent> ciLoad(const InputPacket& packet);
};

}// end namespace ph