#pragma once

#include "Actor/Material/SurfaceMaterial.h"
#include "Core/SurfaceBehavior/SurfaceOptics/TranslucentMicrofacet.h"
#include "Common/primitive_type.h"
#include "Math/TVector3.h"
#include "FileIO/SDL/TCommandInterface.h"

namespace ph
{

class AbradedTranslucent : public SurfaceMaterial, public TCommandInterface<AbradedTranslucent>
{
public:
	AbradedTranslucent();
	~AbradedTranslucent() override;

	void genSurface(CookingContext& context, SurfaceBehavior& behavior) const override;

	//void setAlbedo(const Vector3R& albedo);
	void setIor(const real iorOuter, const real iorInner);
	void setRoughness(const real roughness);

private:
	TranslucentMicrofacet m_optics;

// command interface
public:
	explicit AbradedTranslucent(const InputPacket& packet);
	static SdlTypeInfo ciTypeInfo();
	static void ciRegister(CommandRegister& cmdRegister);
};

}// end namespace ph